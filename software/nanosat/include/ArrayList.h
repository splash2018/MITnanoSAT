/*
 * The Arduino Header ArrayList
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


#ifndef ArrayList_h
#define ArrayList_h
// the #include statment and code go here...
#include "Arduino.h"

class ArrayList {
public:	
  ArrayList(char*); //ArrayStringList(void);
  ~ArrayList(void);
  void display_string_list();
  void add_string_item(char* item);
  void set_string_item(char* item, int index);
  void remove_selected_item(int index);
  void empty_list();
  void set_stringlist(char** stringlist);
  char** get_stringlist();
  char* get_item(int index);
  int get_size();
private:
  char** stringlist;
  int size;	
};

#endif
