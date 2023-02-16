/*
 * This is a simple program showing how to use TinyJS
 */
#include <Arduino.h>
#include <TinyJS.h>
//#include "TinyJS_Functions.h"
#include "_QEspNowScript.h"

//const char *code = "var a = 5; if (a==5) a=4; else a=3;";
//const char *code = "{ var a = 4; var b = 1; while (a>0) { b = b * 2; a = a - 1; } var c = 5; }";
//const char *code = "{ var b = 1; for (var i=0;i<4;i=i+1) b = b * 2; }";
const char *code = "function myfunc(x, y) { return x + y; } var a = myfunc(1,2); print(a);";

void js_print(CScriptVar *v, void *userdata)
{
   Serial.printf("> %s\n", v->getParameter("text")->getString().c_str());
}

void js_dump(CScriptVar *v, void *userdata)
{
   CTinyJS *js = (CTinyJS *)userdata;

   js->root->trace(">  ");
}

void js_load(CScriptVar *v, void *userdata)
{ 
   char cPath[32];
   sprintf(cPath, "/%s",  v->getParameter("text")->getString().c_str());
   String sCode = readScriptFile(cPath);
   //Serial.println(sCode);
   CTinyJS *js = (CTinyJS *)userdata;
   js->execute(sCode.c_str());
}

void myTinyJS()
{
 CTinyJS *js = new CTinyJS();
   /* add the functions from TinyJS_Functions.cpp */
   //registerFunctions(js);
   /* Add a native function */
   js->addNative("function print(text)", &js_print, 0);
   js->addNative("function dump()", &js_dump, js);
   js->addNative("function load(text)", &js_load, js);
   
   /* Execute out bit of code - we could call 'evaluate' here if we wanted something returned */
   js->execute("var lets_quit = 0; function quit() { lets_quit = 1; }");
   
   //Interactive JS
   js->execute("print(\"Interactive mode... Type quit(); to exit, load(...); to load .js file, or print(...); to print something, or dump() to dump the symbol table!\");");
   char cBuf[256];
   uint8_t iCount=0;
   while (js->evaluate("lets_quit") == "0") {
      if (Serial.available() > 0){
         Serial.read(cBuf+iCount, sizeof(uint8_t));
         Serial.write(cBuf[iCount]);
         if (cBuf[iCount]=='\n') {
            cBuf[iCount]=0;
            if (iCount>1) {
               Serial.print("\nTinyJS:");
               Serial.printf("%s", (const char *)cBuf);
               Serial.print("\nTinyJS>");
               js->execute((const char*)cBuf); 
               iCount=0;//reset code buffer counter
               }
            }
         else
            iCount++;  
         }
      yield();   
      }
   delete js;
}