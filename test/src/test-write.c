#include "test-common.h"

static int write_cb_called;

static void write_client() {
  char buf[1024];

  CHECK_EQ(SSL_read(client.ssl, buf, sizeof(buf)), 5, "SSL_read() == 5");
  CHECK_EQ(strncmp(buf, "hello", 5), 0, "write_client data match");
}


static void write_cb(uv_link_t* link, int status) {
  CHECK_EQ(status, 0, "write_cb() status");
  CHECK_EQ(link, &server.observer.link, "write_cb() link must be server");

  write_cb_called++;
}


static void write_server() {
  uv_link_t* serv;
  uv_buf_t buf;

  serv = &server.observer.link;

  CHECK_EQ(uv_link_read_stop(serv), 0, "uv_link_read_stop(server)");

  buf = uv_buf_init("hello", 5);
  CHECK_EQ(uv_link_write(serv, serv, &buf, 1, NULL, write_cb), 0,
           "uv_link_write(server)");

  CHECK_EQ(uv_run(loop, UV_RUN_DEFAULT), 0, "uv_run()");
  /* TODO(indutny): check number of write_cb's once they will be called */
  /* CHECK_EQ(write_cb_called, 1, "number of write_cb's"); */
}


TEST_IMPL(write) {
  ssl_client_server_test(write_client, write_server);
}
