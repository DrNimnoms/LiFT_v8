
/*------------------------------------------------------------------------------
 * void setContactors(void))
 * sets the contactors on or off
 *----------------------------------------------------------------------------*/
void setContactors(){
  if(contactorsOn) relayOn(); 
  else relayOff();
}
/*------------------------------------------------------------------------------
 * void relayOn(void))
 * turns both relays on with 1 sec delay between
 *----------------------------------------------------------------------------*/
void relayOn(void){
  digitalWrite(relay1, HIGH);
  if(conOnTime<5) conOnTime++;
  else digitalWrite(relay2, HIGH);
}

/*------------------------------------------------------------------------------
 * void relayOff(void))
 * turns both relays off and sets currentOffset
 *----------------------------------------------------------------------------*/
void relayOff(void){
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);
  if(conOnTime<5) conOnTime++;
  else currentOffset=0.9*currentOffset+0.1*curMeas;
}
