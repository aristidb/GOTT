// $Id: entry.c,v 1.6 2004/12/11 21:44:22 ltjax Exp $


#ifdef _glsk_load_base_
#endif // _glsk_load_base_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#if defined(WIN32) || defined(WINDOWS_SYSTEM)
#include <windows.h>

typedef char* pchar;
int      glsk_user_main( int argc, char **argv );

int WINAPI WinMain( HINSTANCE hinstance, HINSTANCE hlastinstance, PSTR cmdline, int iCmdShow )
{
   /*int length = strlen( cmdline );

   for ( int i = 0; i < length, ++i )
   {

   }*/

   //TODO: parse the string

   pchar   strings[ 1 ];

   strings[ 0 ] = "glsk";

   return glsk_user_main( 1, strings );
}

#else // X-Windows


int main ( int argn, char** argv )
{
  return glsk_user_main( argn, argv );
}

//glsk function implementations

#endif

