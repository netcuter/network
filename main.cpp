// TODO : wielowątkowość, możliwość wysyłania na STDIN procesu, zoptymalizować kod (pisany na szybko)
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <vector>

#define MAX_SIZE_POLECENIA 256

using namespace std;

    class Zadanie
    {
    public:
        int id;
        char Polecenie[MAX_SIZE_POLECENIA];

    };

    class MenadzerZadan
    {
        public:
        vector <Zadanie> Zadania;
        int DodajZadanie(Zadanie *zadanie);
        int UsunZadanie(unsigned int nr);
    };

    class Polaczenie
    {
    public:
        Polaczenie();
        int Polacz(char *IP, unsigned int port);
        int Rozlacz();
        int Zaloguj();
        int Odbieraj();
        int Wyslij(char *pakiet);
        bool Polaczono;
        bool debug_mode;
    private :
        int sock_descriptor;
        struct sockaddr_in serwer_addr;
        struct hostent *server;
        char pakiet_od_serwera[MAX_SIZE_POLECENIA];
        char pakiet_do_serwera[MAX_SIZE_POLECENIA];
        
    };

    class Agent
    {
    public:
		Agent();
        Polaczenie polaczenie;
        int WykonajZadanie(Zadanie * zadanie);
		char nazwahosta[255];
		unsigned int port;
		unsigned int timeout;
		bool debug_mode;
		void Pomoc();
    };


MenadzerZadan MZadan;



    int MenadzerZadan::DodajZadanie(Zadanie *zadanie)
    {

        Zadanie zad;
        //zad.id=zadanie->id;
        zad.id=Zadania.size();
        strncpy(zad.Polecenie,zadanie->Polecenie,MAX_SIZE_POLECENIA);
        Zadania.push_back(zad);
        return 1;
    }

    int MenadzerZadan::UsunZadanie(unsigned int nr)
    {
        Zadania.resize(Zadania.size()-1);
        return 1;
    }

    int Agent::WykonajZadanie(Zadanie *zadanie)
    {
        FILE *STDOUT_procesu;
                char bufor [1035];
                printf(zadanie->Polecenie);
                STDOUT_procesu = popen(zadanie->Polecenie,"r");
         if (!STDOUT_procesu)
         {
           printf("Nie udalo sie uruchomic polecenia\n" );
           exit;
         }

         while (fgets(bufor, sizeof(bufor)-1, STDOUT_procesu) && this->polaczenie.Polaczono)
         {
             printf("Wyslij(");
             printf(bufor);
             polaczenie.Wyslij(bufor);
             printf(")");
         }
         strncpy(bufor,"Wykonano ", MAX_SIZE_POLECENIA+10);
         strcat(bufor,zadanie->Polecenie);
         polaczenie.Wyslij(bufor);
         if(STDOUT_procesu)
            pclose(STDOUT_procesu);
        MZadan.UsunZadanie(zadanie->id);
    }

    Polaczenie::Polaczenie()
    {
        this->Polaczono=false;
    }

    int Polaczenie::Polacz(char *IP, unsigned int port)
    {
        sock_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        if(sock_descriptor < 0)
          printf("Nie udalo sie utworzyc socketu\n");
        bzero((char *)&serwer_addr, sizeof(serwer_addr));
        server = gethostbyname(IP);
        if(!server)
        {
            printf("Nie znaleziono nazwy serwera\n");
            return -1;
        }
        serwer_addr.sin_family = AF_INET;
        memcpy((char *) &(serwer_addr.sin_addr.s_addr), (char *)(server->h_addr), server->h_length);
        serwer_addr.sin_port = htons(port);
        if (connect(sock_descriptor, (struct sockaddr *)&serwer_addr, sizeof(serwer_addr)) < 0)
        {
            printf("Polaczenie z serwerem nieudane, proba polaczenia ponownie\n");
            return 0;
        }
        else
        {
             printf("Polaczono\n");
             this->Polaczono=true;
            return 1;
        }

    }

    int Polaczenie::Rozlacz()
    {
        this->Polaczono = false;
        return shutdown(this->sock_descriptor,2);
    }

    int Polaczenie::Zaloguj()
    {
        return this->Wyslij("HELLO\0");
    }

    int Polaczenie::Odbieraj()
    {
        int ile = read(sock_descriptor,pakiet_od_serwera,MAX_SIZE_POLECENIA);
        if(ile>0 && ile < MAX_SIZE_POLECENIA)
        {
               // int i = 0;
                //for(; pakiet_od_serwera[i]!='\n' && i<= strlen(pakiet_od_serwera); ++i)
                //    ;
                pakiet_od_serwera[ile]='\0';
                printf(pakiet_od_serwera);
                Zadanie * temp = new Zadanie;
                strncpy(temp->Polecenie,pakiet_od_serwera,ile+1);
                MZadan.DodajZadanie(temp);
                delete temp;
            return 1;
        }
        else
        {
            this->Rozlacz();
            return 0;

        }
    }

    int Polaczenie::Wyslij(char *pakiet)
    {
        int count = -1;
      //  if(sock_descriptor)
            //count = write(sock_descriptor, pakiet, strlen(pakiet));
        count = send(sock_descriptor, pakiet, strlen(pakiet),MSG_NOSIGNAL);
        if(count < 0)
        {
            printf("Nie udalo sie wyslac do serwera\n");
            this->Polaczono=false;
            return -1;
        }
        else
            if(!count)
                return 0;
            else
                return count;
    }
    
    Agent::Agent()
    {
		timeout=5;
		debug_mode=0;
		polaczenie.debug_mode=0;
	}
    void Agent::Pomoc()
    {
		puts("Uzycie : Agent [-l nazwa_hosta] [-p port] [-w timeout ] [-v]");
	}

int main(int argc, char *argv[])
{
  Agent * agent = new Agent;
  
  if(argc<3)
  {
	agent->Pomoc();
	return 1;
  }
  else
  {
   unsigned short int ilosc_argumentow_niezbednych=0;
   for(int i=1; i<argc;++i)
   {
	   if(!strcmp(argv[i],"-l"))
	   {
		   i++;
		   if(i>=argc)
		   {
			    agent->Pomoc();
				puts("Brakuje argumentu dla opcji -l");
				return 1;
		   }
		   else 
		   {
			   strncpy(agent->nazwahosta,argv[i],255);
			   ilosc_argumentow_niezbednych++;
		   }
	   }
	   else
	   {
		   if(!strcmp(argv[i],"-p"))
		   {
			   i++;
			   if(i>=argc)
			   {
					agent->Pomoc();
					puts("Brakuje argumentu dla opcji -p");
					return 1;
			   }
			   else
			   {
					int port=atoi(argv[i]);
					if( port>0 && port <=65535)
					{
						agent->port=port;
						ilosc_argumentow_niezbednych++;
					}
					else
					{
						puts("Podano nieprawidlowy nr portu");
						return 1;
					}
			   }	 
		   }
		   else
				if(!strcmp(argv[i],"-w"))
				{
					i++;
				    if(i>=argc)
				    {
					 	agent->Pomoc();
						puts("Brakuje argumentu dla opcji -w");
						return 1;
				    }
				    else
				    {
						int w=atoi(argv[i]);
						if(w>0)
							agent->timeout=w;
						else
						{
							puts("Podano nieprawidlowa wartosc timeoutu");
							return 1;
						}
					}
				}
				else
					if(!strcmp(argv[i],"-v"))
					{
						agent->debug_mode=1;
						agent->polaczenie.debug_mode=1;
					}
		
	   }
   }
   
   if(ilosc_argumentow_niezbednych>=2)
   while(1>0)
   {
       if(agent->polaczenie.Polacz(agent->nazwahosta,agent->port))
       {
           agent->polaczenie.Zaloguj();
           while(agent->polaczenie.Polaczono)
           {
               if(agent->polaczenie.Odbieraj())
                   if(MZadan.Zadania.size() > 0)
                    agent->WykonajZadanie(&MZadan.Zadania[0]);
           }
       }
       sleep(agent->timeout);
   }
   else agent->Pomoc();
   
  }
  delete agent;
  return EXIT_SUCCESS;
}
