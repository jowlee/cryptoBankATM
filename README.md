# cryptoBankATM

## Setup and Running the Program:

Run the “make” command to compile the files  
Run “./ATMserver <bankPortNumber>” to start the bank  
Run “./ATMproxy <clientPortNumber> <bankPortNumber>” to start the proxy  
Run “./ATMclient <clientPortNumber>” to start the client  

## Users:
user: alice	password: abc123  
user: bob	password: def456  
user: eve	password: ghi789  

## Usage: Example useage

User A: atm ~ : login alice  
User A: password: abc123  
User A: login in  

User A: atm ~ : balance  
User A: Obtaining balance ...  
User A: balance is 50   

User A: atm ~ : withdraw 25  
User A: withdrew  

User A: atm ~ : transfer bob 20  

User A: atm ~ : logout  
User A: closing  
