# TODO

## NGINX
Use a unix socket to passin websocket traffic.

```nginx
location ~ /socket {
  proxy_pass http://unix:/tmp/app.socket;
  proxy_http_version 1.1;
  proxy_set_header Upgrade "websocket";
  proxy_set_header Connection "upgrade";
}
```