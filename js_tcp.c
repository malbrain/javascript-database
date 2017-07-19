#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <io.h>

#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>
#define closesocket close
#define SOCKET int
#endif

#ifdef _DARWIN
#include <unistd.h>
#endif

#include <fcntl.h>

#include "js.h"

typedef struct {
	SOCKET conn_fd;
	value_t conn_id;
	closure_t *closure;
	environment_t env[1];
} param_t;

#ifdef _WIN32

DWORD WINAPI js_tcpLaunch(param_t *config) {
#else
void *js_tcpLaunch(void *arg) {
	param_t *config = arg;
#endif
	value_t args = newArray(array_value, 3);
	array_t *aval = args.addr;
	char outbuff[32768];
	value_t fcn, thisVal;
	value_t fin, fout;
#ifdef _WIN32
	int fd = _open_osfhandle(config->conn_fd, O_BINARY);
	fin.bits = vt_file;
	fin.file = _fdopen (fd, "rb");

	fout.bits = vt_file;
	fout.file = _fdopen (fd, "r+b");
#else

	fin.bits = vt_file;
	fin.file = fdopen (config->conn_fd, "rb");

	fout.bits = vt_file;
	fout.file = fdopen (config->conn_fd, "r+b");
#endif

	setvbuf(fout.file, outbuff, _IOFBF, sizeof(outbuff));

	thisVal.bits = vt_undef;

	fcn.bits = vt_closure;
	fcn.closure = config->closure;

	aval->valuePtr[0] = fin;
	aval->valuePtr[1] = fout;
	aval->valuePtr[2] = config->conn_id;

	config->env->timestamp = newTsGen();

	fcnCall (fcn, args, thisVal, false, config->env);

	js_free(config->env->timestamp);
	fclose(fin.file);
#ifdef _WIN32
	return true;
#else
	return NULL;
#endif
}

value_t js_tcpListen(uint32_t args, environment_t *env) {
	SOCKET conn_fd, listen_fd;
	struct sockaddr_in sin[1];
	uint64_t conn_id = 0;
	socklen_t sin_len[1];
	value_t port, fcn, v;
	param_t *params;
	int opt[1];
	int err;

	v.bits = vt_status;

#ifdef _WIN32
	WSADATA sock_data[1];
	DWORD thread_id[1];
	HANDLE thrd;
#else
	pthread_t thread_id[1];
#endif

#ifdef _WIN32
	memset (sock_data, 0, sizeof(sock_data));

	if ((err = WSAStartup(MAKEWORD(2,2), sock_data))) {
		fprintf(stderr, "WSAStartup error: %d\n", err);
		exit(1);
	}

	*opt = 0x20; // SO_SYNCHRONOUS_NONALERT;
	if (setsockopt(INVALID_SOCKET, SOL_SOCKET, SO_OPENTYPE, (const char *)opt, sizeof opt)) {
		fprintf(stderr, "setsockopt error: %d\n", WSAGetLastError());
		exit(1);
	}
#endif

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);

#ifdef _WIN32
	if (listen_fd == INVALID_SOCKET)
		return v.status = ERROR_tcperror, v;
#else
	if (listen_fd < 0)
		return v.status = ERROR_tcperror, v;
#endif

	port = eval_arg(&args, env);
	fcn = eval_arg(&args, env);

	if (fcn.type != vt_closure) {
		fprintf(stderr, "tcpListen Error: expected fcn closure %s\n", strtype(fcn.type));
		exit(1);
	}

	incrRefCnt(fcn);

	memset (sin, 0, sizeof(*sin));

	sin->sin_family = AF_INET;
	sin->sin_port = htons((unsigned short)conv2Int(port, true).nval);

	*opt = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, (const char *)opt, sizeof opt);

	err = bind(listen_fd, (const struct sockaddr *)sin, sizeof(*sin));
	if (err) {
#ifdef _WIN32
		fprintf (stderr, "tcpbind error: %d\n", WSAGetLastError());
#else
		fprintf (stderr, "tcpbind error: %d\n", err);
#endif
		return v.status = ERROR_tcperror, v;
	}

	err = listen (listen_fd, 12);

	if (err)
		return v.status = ERROR_tcperror, v;

	setsockopt(listen_fd, SOL_SOCKET, SO_KEEPALIVE, (const char *)opt, sizeof opt);

	do {
		*sin_len = sizeof(*sin);
		conn_fd = accept(listen_fd, (struct sockaddr *)sin, sin_len);

#ifdef _WIN32
		if (conn_fd == INVALID_SOCKET )
			return v.status = ERROR_tcperror, v;
#else
		if (conn_fd < 0 )
			return v.status = ERROR_tcperror, v;
#endif
		*opt = 1;
		setsockopt(conn_fd, IPPROTO_TCP, TCP_NODELAY, (const char *)opt, sizeof opt);

		params = js_alloc (sizeof(*params), false);
		memcpy (params->env, env, sizeof(environment_t));
		params->env->timestamp = newTsGen();

		params->conn_id.bits = vt_int;
		params->conn_id.nval = ++conn_id;
		params->closure = fcn.closure;
		params->conn_fd = conn_fd;

#ifdef _WIN32
		thrd = CreateThread(NULL, 0, (PTHREAD_START_ROUTINE)js_tcpLaunch, params, 0, thread_id);
		CloseHandle (thrd);
#else
		if (pthread_create(thread_id, NULL, js_tcpLaunch, params))
			return v.status = ERROR_tcperror, v;
#endif
	} while( conn_fd > 0 );

	closesocket(listen_fd);
	return v.status = ERROR_tcperror, v;
}
