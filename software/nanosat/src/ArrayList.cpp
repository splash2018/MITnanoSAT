/*
 * Arduino Class for ArrayList
 * Written: Obed Isai Rios
 */

//#######################################
//#                                     #
//#     DALE PY GROUP                   #
//#     Mr Panetone                     #
//#      marcelu.phd@gmail.com          #
//#                                     #
//#     01/08/2019                      #
//#######################################



#include "Arduino.h"
#include "ArrayList.h"

ArrayList::ArrayList(char* init){
  stringlist = (char**)malloc(10*sizeof(char*));
  stringlist[0] = init;
  this->size = 1; 
}

ArrayList::~ArrayList(void)
{
}

void ArrayList::add_string_item(char* item){
  char **neulist = (char**)malloc((size+1)*sizeof(char*));
  for(int i=0; i<size; i++){
    neulist[i] = stringlist[i];
   }
   //
   neulist[size] = item;
   stringlist = neulist;
   size = size + 1;
}

void ArrayList::set_string_item(char* item, int index){
  stringlist[index] = item;
}

void ArrayList::remove_selected_item(int index){
  char **neulist = (char**)malloc((size-1)*sizeof(char*));
  //From Begining
  for(int i=0; i<index; i++){
    neulist[i] = stringlist[i]; 
  }
  //From next Index  
  for(int i=index; i<=size-1; i++){
    neulist[i] = stringlist[i+1];
  }
  //free(matrix);
  stringlist = neulist;
  size = size - 1;
}

void ArrayList::empty_list(){
   size = 1;
   char **neulist = (char**)malloc((size)*sizeof(char*));   
   stringlist = neulist;
   stringlist[0] = "EMPTY";
}

void ArrayList::display_string_list(){
   Serial.begin(9600);
  for(int i=0; i<size; i++){
    Serial.println(stringlist[i]);
   }
}

char** ArrayList::get_stringlist(){
  return this->stringlist;
}

char* ArrayList::get_item(int index){
  return this->stringlist[index];
}

void ArrayList::set_stringlist(char** stringlist){
  this->stringlist = stringlist;
}

int ArrayList::get_size(){
  return this->size;
}

/*
#include "ArrayList.h"

 ArrayList *list = new ArrayList("Hello World");
  list->add_string_item("From my Little World");
  list->add_string_item("of Arduino");
  list->add_string_item("Added Portion #1");   
  list->add_string_item("Added Portion #2");   
  list->add_string_item("Added Portion #3");   
  list->display_string_list();  
  Serial.println("-");
  delay(EEPROM_DELAY);
  list->remove_selected_item(4);
  list->display_string_list();
  Serial.println("-");
  delay(EEPROM_DELAY);
  list->empty_list();
  list->set_string_item("Hello Empty World",0);
  char* str = list->get_item(0);
  Serial.println(str);
  list->display_string_list();
  delete list;
 */