#include "API.h"
#include <iomanip>
#include <signal.h>
#include <thread>
#include <mutex>

API api;
void exit_handler(int s);
int main()
{
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = exit_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  bool cont = true;
  printf("\033[2J");
  while(cont)
  {
    printf("Please choose what to do \n");
    printf(" Press \033[31;1ms\033[0m to Limit Sell all assets currency \n");
    printf(" Press \033[31;1mS\033[0m to Market Sell all assets currency \n");
    printf(" Press \033[32;1mb\033[0m to Limit Buy using all fiat currency \n");
    printf(" Press \033[32;1mB\033[0m to Market Buy using all fiat currency \n");
    printf(" Press d to get balances \n");
    printf(" Press o to Cancel All Orders\n");
    printf(" Press x to Exit \n");
    printf("Choice: \n");
    char ch = 0;
    cin >> ch;
    printf("\033[2J");
    switch(ch)
    {
      case 's':
        api.Insta_Limit_Sell();
        break;
      case 'S':
        api.Insta_Market_Sell();
        break;
      case 'b':
        api.Insta_Limit_Buy();
        break;
      case 'B':
        api.Insta_Market_Buy();
        break;
      case 'd':
        api.Show_Balances();
        break;
      case 'o':
        cout << "\033[36mDeleting all Orders.\033[0m\n";
        api.Delete_All_Orders();
        break;
      case 'X':
      case 'x':
        cont = false;
        break;
    }
  }
  return 0;
}

void exit_handler(int s){
  api.Delete_All_Orders();
  cout << endl << "Caught Exit Signal." << s << endl;
  cout << "Killed All Orders";
  cout << endl;
  exit(EXIT_SUCCESS);
}
