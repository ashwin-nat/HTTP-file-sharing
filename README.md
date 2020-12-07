# HTTP file sharing server

This is a file sharing server that runs on HTTP. It will list the files and 
directories in it's working directory, and the client can choose to download a 
specific file. 
Currently, only HTTP GET method is supported.
As of now, every connection will be closed once the request has been served (no 
persistent connections)

## Features

* Configurable working directory (using -d option)
* Concurrent connections (using a thread pool of configurable size: -t option)
* Zero copy file send (using sendfile)

## Building

use GNU make

```
make
```

## Portability

This code is mostly portable. As of now, the platform specific functionalities 
are:

* poll ()
* MSG_NOSIGNAL flag for send ()
* Unix sockets (not supported on windows)
* sendfile ()

## Issues

This server is not very secure, and I'm not talking about the fact that it's 
not running on HTTPS. This server is prone to simple Denial-of-Service attacks. 
Please ensure that you don't run this as root. If you do need to bind it to 
port 80, you can set the capabilities of the server using

```
sudo setcap cap_net_bind_service=ep <path-to-binary>
```

## Credits

* [loguru] (<https://github.com/emilk/loguru>)

## License

[MIT] (<https://choosealicense.com/licenses/mit/>)