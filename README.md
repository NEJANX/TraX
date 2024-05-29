
### Instructions

1.  **Compile the Server**:
		```bash 
		gcc TraX_Server.c -o TraX_Server -lpthread
		```
2.   **Run the Server**:
   		```bash
		./TraX_Server
		```
    
3.   **Compile the Client**:
		```bash 
		gcc TraX.c -o TraX -lpthread
		```
    
4.   **Run the Client**:
    
    
		```bash
		./TraX <server_ip>
		```

		Replace `<server_ip>` with the IP address of the server, for example:



		```bash
		./TraX 127.0.0.1
  	 ```
