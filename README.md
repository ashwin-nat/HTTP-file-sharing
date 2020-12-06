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

## Credits

* [loguru] (https://github.com/emilk/loguru)

## License
[MIT](https://choosealicense.com/licenses/mit/)