Project Title

Building a Web Server. Programming in C to execute the HTTP commands (GET, PUT ,HEAD, DELETE) through terminal, browser and postman.

Getting Started

The Web server program can be executed from terminal , Chrome or postman

	A) Execution through Terminal

		Follow the given instructions to get you a copy of the project up and running on your local machine for development 
		and testing purposes

			step 1: Copy the entire server on your local machine.
			
			a) Starting the server
			
			step 2: Open terminal and set the path to the server file

			step 3: compile server source code file ,i.e server.c 
			compile code: gcc -g server.c 

			step 4: execute the server.c file with following command
			
				./a.out IP_address Port_number(between 2000 to 6000)
				example ./a.out 192.168.0.2 2204

			b) Establishing client-server connection
			
			step 1: execute the client using telnet with following command
			
					telnet IP_address Port_number(between 2000 to 6000)
					example telnet 192.168.0.2 2204   
				
			IP adress and port number should be the same as used for server
			This will connect the client to the server. 
		
			Once the connection is established use the HTTP commands GET,PUT,HEAD,DELETE
		
			GET command
		
			step 4: from client terminal use command GET /file_name.extension and hit return
					example GET /Hello.txt 

			The file that we are requesting should be present in the server directory. IF the file is not present then the 
			server will send error 404 file not found.If the file is present then the content of the file will be displayed 
			on the terminal and connection will be closed on the client side
		
			To request another file from server, client should follow step 3 and step 4 of b
		
			PUT command
			
			step 4: from client terminal use command PUT /file_name.extension and hit return
					example PUT /Hello.txt 
			step 5: Write upto 1024 bytes which you need to put on server and hit return

			If Hello.txt is present on server then its content will be modified. IF the file is not present then new file will 
			be created on server side and the content will be copied in it. Manually check if Hello.txt is created.Connection will 
			be closed on client side
		
		    To put another file from server, client should follow step 3 of b and step 4-5 of PUT command
		    
		    HEAD command
		    
		    step 4: from client terminal use command HEAD /file_name.extension and hit return
					example HEAD /Hello.txt 

			The file that we are requesting should be present in the server directory. IF the file is not present then the 
			server will send error 404 file not found.If the file is present then the file information is displayed on client 
			side and connection will be closed
		
			To request another file from server, client should follow step 3 of b step 4 of HEAD
			
			DELETE command
		    
		    step 4: from client terminal use command DELETE/file_name.extension and hit return
					example DELETE /Hello.txt 

			The file that we are requesting should be present in the server directory. IF the file is not present then the 
			server will send error 404 file not found.If the file is present then the file will be deleted from server directory.
			Manually check if file is deleted from server
	
		
			To DELETE another file from server, client should follow step 3 of b step 4 of HEAD
		
	B) Execution through Browser(chrome)
	
		Chrome can handle only GET request 
			
			a) Starting the server
			
			step 1: Copy the server folder on the local machine
			step 2: Open terminal and set the path to the server file
			step 3: compile server source code file ,i.e server.c 
			step 4: execute the server.c file with following command
			
				./a.out IP_address Port_number(between 2000 to 6000)
				example ./a.out 192.168.0.2 2204
				
			b) Open chrome browser
			
			step 1: enter url IP_adress:port_number/file_name.extension and hit return
					example 192.168.0.5:2204/Hello.html
					
					IP adress and port number must be same as server
			
			This will display the Hello.html page on the browser.
			
	C) Execution through Postman
	
		Requirement: Postman should be installed
	
			a) Starting the server
			
			step 1: Copy the server folder on the local machine
			step 2: Open terminal and set the path to the server file
			step 3: compile server source code file ,i.e server.c 
			step 4: execute the server.c file with following command
			
				./a.out IP_address Port_number(between 2000 to 6000)
				example ./a.out 192.168.0.2 2204
				
			b) Open Postman
			
				GET command
			
				step 1: choose request GET 
				step 2: enter IP IP_adress:port_number/file_name.extension and click send
					example 192.168.0.5:2204/Hello.html
					
				If file is present the file content will be displayed in the Response window ,otherwise the status code will be 
				displayed on the right side of the Response header
				
				PUT command
			
				step 1: choose request PUT
				step 2: enter IP IP_adress:port_number/file_name.extension
						example 192.168.0.5:2204/Hello.txt
				step 3: From key tab select the option file 
				step 4: From the Value tab choose the file to be PUT i.e Hello.txt and click send
				
					
				File will be copied on the server directory and status code will be displayed on the right side of the Response
				window.
				
				DELETE command
			
				step 1: choose request DELETE 
				step 2: enter IP IP_adress:port_number/file_name.extension and click send
					example 192.168.0.5:2204/Hello.html
					
				If file is present and have access permission then it will be deleted. Otherwise, status code will be displayed
			    on the right side of the Response window.
			
				HEAD command
			
				step 1: choose request HEAD 
				step 2: enter IP IP_adress:port_number/file_name.extension and click send
					example 192.168.0.5:2204/Hello.html
					
				If file is present the file information will be displayed in the Response window under Header tab.Otherwise, the 
				status code will be displayed on the right side of the Response window.
				

Author

Vishakha Supekar




			    
			    
			    
			
					
			
			
		    
		    
		    
		    
		     
		

