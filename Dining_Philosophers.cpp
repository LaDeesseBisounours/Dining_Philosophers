//AUTHOR : Maxime MEHIO 20168146 
#include <cstdlib>
#include <iostream>
#include <cerrno>
#include <unistd.h>

using namespace std;

#include "mpi.h"

//solving dining philosophers problem using Dijkstra solution.
int main ( int argc, char *argv[] ) 
{
  int id; //my MPI ID
  int p;  //total MPI processes
  MPI::Status status;
  int tag = 1;

  //  Initialize MPI.
  MPI::Init ( argc, argv );

  //  Get the number of processes.
  p = MPI::COMM_WORLD.Get_size ( );

  //  Determine the rank of this process.
  id = MPI::COMM_WORLD.Get_rank ( );
  
  //Safety check - need at least 2 philosophers to make sense
  if (p < 3) {
	    MPI::Finalize ( );
	    std::cerr << "Need at least 2 philosophers! Try again" << std::endl;
	    return 1; //non-normal exit
  }

  sleep(rand()%5);
  srand(id + time(NULL)); //ensure different seeds...

  
  //  Setup Fork Master (Ombudsman) and Philosophers
  if ( id == 0 ) //Master
  {
	  int msgIn; //messages are integers
    int philosophers_left = p-1;
	  
    int *fork;
    int philosopher;
    int rank;
    fork = (int*)std::calloc(p-1,sizeof(int));
    while(philosophers_left >0){
      //while some philosophers haven 't got their number of maximum meals
      MPI::COMM_WORLD.Recv(&msgIn, 1, MPI::INT, MPI::ANY_SOURCE, tag, status);
      philosopher = status.Get_source();
      std::cout << "Received message "<<msgIn<<" from philosopher "<< 
          philosopher<<"."<<std::endl;
      //send some response depending on message received.
      switch (msgIn){
        //philosopher got his maximum number of meals.
        case 0:
            philosophers_left--;
          break;
        //philosopher wants his lower rank fork.
        case 1:
          if(philosopher == p){
            rank = 0;
          }
          else{
            rank = philosopher -1;
          }
          if(*(fork +rank)){
            MPI::COMM_WORLD.Send(&msgIn, 1, MPI::INT, philosopher, tag);
          }
          else{
            *(fork + rank) = 1;
            msgIn = 3;
            MPI::COMM_WORLD.Send(&msgIn, 1, MPI::INT, philosopher, tag);
          }
          break;
        //philosopher finished his meal, gives back both forks.
        case 2:
          *(fork + philosopher -1) = 0;
          *(fork + (philosopher%p)) = 0;
          break;
        //philosopher wants his higher rank fork.
        case 3:
          rank = philosopher;
          if(philosopher == p){
            rank--;
          }
          if(*(fork + (rank))){
            MPI::COMM_WORLD.Send(&msgIn, 1, MPI::INT, philosopher, tag);
          }
          else{
            *(fork + (rank))= 1;
            msgIn = 2;
            MPI::COMM_WORLD.Send(&msgIn, 1, MPI::INT, philosopher, tag);
          }
          break;
      }
    }
    free(fork);
    //to show we finished all and free memory
    std::cout<<"Everything completed !!!"<<std::endl;
  }
  else //I'm a philosopher
  {
    int msgOut;
    int max_meals = 3;
    for(int numberofmeals = 0; numberofmeals < max_meals; numberofmeals ++){
      sleep(rand()%5);
      msgOut = 1;
      std::cout<<"Philosopher "<<id<<" ask for low rank fork."<< std::endl;
      while(msgOut != 3){//loop waiting for low rank fork to be free.
        MPI::COMM_WORLD.Send (&msgOut, 1, MPI::INT, 0, tag);
        MPI::COMM_WORLD.Recv (&msgOut, 1, MPI::INT, 0, tag, status);
        sleep(1);
      }
      std::cout<<"Philosopher "<<id<<" ask for high rank fork."<< std::endl;
      while(msgOut != 2){//loop waiting for high rank fork to be free.
        MPI::COMM_WORLD.Send (&msgOut, 1, MPI::INT, 0, tag);
        MPI::COMM_WORLD.Recv (&msgOut, 1, MPI::INT, 0, tag, status);
        sleep(1);
      }
      std::cout<<"Philosopher "<<id<<" starts eating."<< std::endl;
      sleep(3);
      msgOut = 2;
      MPI::COMM_WORLD.Send (&msgOut, 1, MPI::INT, 0, tag);
      std::cout<<"Philosopher "<<id<<" goes back to doing philosophy."
          << std::endl;
      sleep(5);
    }
    std::cout<<"Philosopher "<<id<<" got his "<<max_meals<< " meals."
        << std::endl;
    msgOut = 0;
    MPI::COMM_WORLD.Send(&msgOut, 1, MPI::INT, 0, tag);
  }
  
  //  Terminate MPI.
  MPI::Finalize ( );
  return 0;
}
