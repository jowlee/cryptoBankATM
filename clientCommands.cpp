// basic file operations
#include <iostream>
#include <fstream>


// Read input until we read a space, then for each character add it to the command string
std::string advanceCommand(const std::string& line, int &index) {
  std::string command = "";
  for(; line[index] != ' ' && index <= line.length(); index++) {
    command += line[index];
  }
  return command;
}
// Skip any number of spaces
void advanceSpaces(const std::string &line, int &index) {
  for(; line[index] == ' ' && index <= line.length(); index++);
}



void login(const std::string username){

  std::string cardInfo;
  std::string cardName = username + ".card";
  std::cout << "Cardname is " << cardName << std::endl;
  std::fstream myfile;
  myfile.open (cardName);
  getline(myfile,cardInfo); // Saves the line in cardInfo.
  myfile.close();

  // Do checks

  return "workds";



  // Ask for pin and check length is 6



}

			// 	if (card_read != AES_KEY_LENGTH){
			// 		printf("Error while reading card. Try again\n");
			// 		continue;
			// 	}
      //
			// 	card_key.Assign((const byte*) card_contents, AES_KEY_LENGTH);
      //
			// 	//encrypt session key using card key
      //
			// 	char enc_session_key[AES_KEY_LENGTH];
      //
			// 	encrypt_session_key(session_key, card_key, enc_session_key);
      //
			// 	if (enc_session_key == NULL){
			// 		printf("Error encrypting session key\n");
			// 		continue;
			// 	}
      //
			// 	//read 20 characters after "login username[space], (PIN)"
			// 	//strncpy(pin,buf+7+strlen(username), 20);
      //
			// 	//don't bother with pin if in debug	 But now do bother
      //
			// 	pin = getpass("\nENTER PIN PLEASE> "); //no echo
      //
      //
			// 	if(!validate_pin(pin)){
			// 		printf("Sorry, that's not a valid pin number. Please try again.\n");
			// 		continue;
			// 	}
      //
			// 	snprintf(packet, PACKET_REMAINING(packet), "%s_login_%s_%s_%s", new_nonce, username, pin, enc_session_key);
      //
			// 	//E_ATM_key (nonce + login + name + pin + E_card_key(session_key))
      //
			// 	sendret = packet_send(packet, sock, atm_private_key, iv);
      //
      //
			// }
