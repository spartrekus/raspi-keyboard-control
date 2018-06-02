                     




#include <stdio.h>                                                                                        
                    #include <sys/select.h>                                                                                   
   #include <sys/time.h>                                                                                     
                       #include <sys/types.h>                                                                                    
                                        #include <sys/stat.h>                                                                                     
                                        #include <sys/ioctl.h>                                                                                    
                                        #include <fcntl.h>                                                                                        
                                        #include <unistd.h>                                                                                       
                                        #include <stdlib.h>                                                                                       
                                        #include <time.h>                                                                                         
                                        #include <linux/input.h>                                                                                  
                                        #include <signal.h>                                                                                       
                                        #include <string.h>                                                                                       
                                        #include <errno.h>                                                                                        
                                                                                                                                                  
                                                                                                                                                  
                                        #include <curses.h>                                                                                       
                                                                                                                                                  
                                        static int fd;                                                                                            
                                        static int own_dev = 1;                                                                                   
                                        int grab = 1;                                                                                             
                                                                                                                                                  
                                        void catch_signal(int signal)                                                                             
                                        {                                                                                                         
                                          switch ( signal ) {                                                                                     
                                          case SIGINT:                                                                                            
                                            /* release device if needed */                                                                        
                                            if ( own_dev ) {                                                                                      
                                              ioctl(fd, EVIOCGRAB, NULL);                                                                         
                                              own_dev = 0;                                                                                        
                                            } else {                                                                                              
                                              ioctl(fd, EVIOCGRAB, &grab);                                                                        
                                              own_dev = 1;                                                                                        
                                            }                                                                                                     
                                            break;                                                                                                
                                          case SIGSTOP:                                                                                           
                                          case SIGQUIT:                                                                                           
                                            exit(0);                                                                                              
                                            break;                                                                                                
                                          }                                                                                                       
                                        }                                                                                                         
                                                                                                                                                  
                                        int main(int argc, char *argv[])                                                                          
                                        {                                                                                                         
                                          fd_set rfds;                                                                                            
                                          int res;                                                                                                
                                          int version = -1, ioret = -1;                                                                           
                                          unsigned numevs, c;                                                                                     
                                          unsigned char read_buffer[sizeof(struct input_event)*3]; /* max 3 events per read */                    
                                          struct input_event *currev;                                                                             
                                          char device_name[1024];                                                                                 
                                          int posy, rows, cols ;                                                                                  
                                                                                                                                                  
                                                                                                                                                  
                                          struct sigaction sighandler;                                                                            
                                          memset(&sighandler, 0, sizeof(sighandler));                                                             
                                          sighandler.sa_handler = catch_signal;                                                                   
                                          sigaction(SIGINT, &sighandler, NULL);                                                                   
                                          sigaction(SIGQUIT, &sighandler, NULL);                                                                  
                                                                                                                                                  
                                          if ( argc < 2 ) {                                                                                       
                                            fprintf(stderr, "Device needed\n");                                                                   
                                            return -1;                                                                                            
                                          }                                                                                                       
                                                                                                                                                  
                                          initscr();                                                                                              
                                          getmaxyx(stdscr, rows, cols);                                                                           
                                                                                                                                                  
                                                                                                                                                  
                                          FD_ZERO(&rfds);                                                                                         
                                          fd = open(argv[1], O_RDONLY);                                                                           
                                          if ( -1 == fd ) {                                                                                       
                                            fprintf(stderr, "unable to read from mice\n");                                                        
                                            return -1;                                                                                            
                                          }                                                                                                       
                                                                                                                                                  
                                          ioret = ioctl(fd, EVIOCGVERSION, &version);                                                             
                                          ioret = ioctl(fd, EVIOCGNAME(sizeof(device_name)), device_name);                                        
                                          ioret = ioctl(fd, EVIOCGRAB, &grab);                                                                    
                                          if ( -1 == ioret ) {                                                                                    
                                            perror("ioctl()");                                                                                    
                                          }                                                                                                       
                                          fprintf(stdout, "ver: %d, ret = %d\n", version, ioret);                                                 
                                          printf("device name is: %s\n", device_name);                                                            
                                          posy = 0 ;                                                                                              
                                          FD_SET(fd, &rfds);                                                                                      
                                                                                                                                                  
                                          erase();                                                                                                
                                          refresh();                                                                                              
                                                                                                                                                  
                                          scrollok(stdscr, TRUE);                                                                                 
                                                                                                                                                  
                                          // loop                                                                                                 
                                          while ( 1 ) {                                                                                           
                                            res = select(fd + 1, &rfds, NULL, NULL, NULL);                                                        
                                            if ( -1 == res && EINTR == errno ) {                                                                  
                                              continue;                                                                                           
                                            }                                                                                                     
                                            if ( -1 == res ) {                                                                                    
                                              perror("select() failed");                                                                          
                                              // fprintf(stderr, "failed to select, fd is %d\n", fd);                                             
                                              return -1;                                                                                          
                                            }                                                                                                     
                                            if ( FD_ISSET(fd, &rfds) ) {                                                                          
                                              //fprintf(stdout, "got some data\n");                                                               
                                              printw(    "error reading data\n");                                                                 
                                                                                                                                                  
                                              res = read(fd, read_buffer, sizeof(read_buffer));                                                   
                                              if ( -1 == res) {                                                                                   
                                                printw( "error reading data\n");                                                                  
                                                return -1;                                                                                        
                                              }                                                                                                   
                                              // fprintf(stdout, "got %d bytes\n", res);                                                          
                                              numevs = ( res / sizeof(struct input_event) ); /* get how many input events we got */               
                                              // fprintf(stdout, "got %u events\n", numevs);                                                      
                                                                                                                                                  
                                                                                                                                                  
                                              for ( c = 0; c < numevs; c++ ) {                                                                    
                                                currev = (struct input_event *)(read_buffer + (sizeof(struct input_event) * c));                  
                                                                                                                                                  
                                                                                                                                                  



  printw( "event time %ld/%ld\n", currev->time.tv_sec, currev->time.tv_usec);                       
  printw( "event type = %hd, code = %hd, value = %d\n", currev->type, currev->code, currev->value); 


  printw ( "Code:  %d \n",  currev->code ) ; 
  printw ( "Value: %d \n",  currev->value ) ; 
                                                                                                                                                  
  if ( currev->code == 104 ) printw( "104 UP  \n" );
  if ( currev->code == 109 ) printw( "109 DOWN  \n" );
                                                                                                                                                  
  /// sound system
  if ( currev->code == 104 ) system( "amixer -q sset Master 2%+"  );
  else if ( currev->code == 109 ) system( "amixer -q sset Master 2%-"  );

  else if ( currev->value == 1 )
  {
   //if ( currev->code ==      72 ) system( " export DISPLAY=:1 ; xdotool key k " );
   //else if ( currev->code == 80 ) system( " export DISPLAY=:1 ; xdotool key j " );
   //if ( currev->code ==      75 ) system( " export DISPLAY=:1 ; xdotool key less " );
   //else if ( currev->code == 77 ) system( " export DISPLAY=:1 ; xdotool key greater " );
   // 75 77    72 80
   if ( currev->code ==      75 ) system( " export DISPLAY=:1 ; xdotool key h " );
   else if ( currev->code == 77 ) system( " export DISPLAY=:1 ; xdotool key l " );

   else if ( currev->code ==      72 ) system( " export DISPLAY=:1 ; xdotool key k " );
   else if ( currev->code == 80 ) system( " export DISPLAY=:1 ; xdotool key j " );

   else if ( currev->code == 78 ) system( " export DISPLAY=:1 ; xdotool key plus " );
   else if ( currev->code == 74 ) system( " export DISPLAY=:1 ; xdotool key minus " );

   else if ( currev->code ==      71 ) system( " export DISPLAY=:1 ; xdotool key less " );
   else if ( currev->code == 73 ) system( " export DISPLAY=:1 ; xdotool key greater " );

   else if ( currev->code == 76 ) system( " export DISPLAY=:1 ; xdotool key space " );

   else if ( currev->code ==      79 ) system( " export DISPLAY=:1 ; xdotool key Left " );
   else if ( currev->code == 81 ) system( " export DISPLAY=:1 ; xdotool key Right " );
   else if ( currev->code == 82 ) system( " export DISPLAY=:1 ; cd ; make  " );

   else if ( currev->code == 98 ) system( " export DISPLAY=:1 ; xdotool key f ; xdotool key Z " );
  }

                                                                                                                                                  
                                                if ( currev->code == 57 ) {                                                                       
                                                                 endwin();                                                                        
                                                     exit(0);                                                                                     
                                                         }                                                                                        
                                                                                                                                                  
                                              }                                                                                                   
                                            } else {                                                                                              
                                              fprintf(stderr, "odd ... no data and we only listen in 1 fd\n");                                    
                                            }                                                                                                     
                                                                                                                                                  
                                                                                                                                                  
                                            // last refresh                                                                                       
                                            refresh();                                                                                            
                                          }                                                                                                       
                                          return 0;                                                                                               
                                        }                                                                                                         
                                                                                                                                                  
