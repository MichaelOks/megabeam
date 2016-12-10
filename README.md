# Megabeam - distributed processing system.

#### C++ open-source software used for distributed processing.
#### MegaBeam is lightweight fast and easy to use system, which consist of a server,
#### unlimited workers and clients. It allows to process a lot of tasks
#### over unlimited computers over TCP/IP.



<br>
# How it works ( in short )
 * clients send any kind of tasks (program) to the server.
 * Server deliver tasks to unbusy workers.
 * workers will receive jobs from the server and execute them.

# How to use?
##### Run as a server
         megabeam server
##### Run as a worker + server ip
         megabeam worker 127.0.0.1 
 
##### A demo client is exist in test folder.
 
 ![Alt text](http://www.ejbtutorial.com/wp-content/uploads/2013/10/computer_network-298x248.jpg "")