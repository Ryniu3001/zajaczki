#include "lamport.h"
#include<stdio.h>

bool compareByClock(const Demand *a, const Demand *b)
{
  if (a->clock < b->clock)
  return true;
  else if (a->clock == b->clock)
  return a->id < b->id;
  return false;
}

void Lamport::addProcess(Demand *t, int polanasId){
  bool alreadyExists = false;
  for (int i=0; i<polany[polanasId].size(); i++){
    if (t->id == polany[polanasId][i]->id){
      alreadyExists = true;
      break;
    }
  }
  if (alreadyExists == false){
    polany[polanasId].push_back(t);
    std::sort(polany[polanasId].begin(),polany[polanasId].end(),compareByClock);
  }
}

void Lamport::addMe(int polanasId, Type type) {
  Demand *demand = new Demand(this->id,this->clock + 1,type); //Clock + 1 bo taka wartosc zostanie wyslana w zadaniu do innych procesow
  addProcess(demand, polanasId);
}

void Lamport::removeProcess(int id, int polanasId) {
  for(int i=0;i<polany[polanasId].size();i++) {
    if(polany[polanasId][i]->id == id) {
      polany[polanasId].erase(polany[polanasId].begin() + i);
      break;
    }
  }
}
/*
bool Lamport::checkPosition(int polanasId) {			//Do czego tego uzywasz?
if(polany[polanasId].size()>0) {
return polany[polanasId][0]->id == id ? true : false;
}
}
*/

bool Lamport::checkPosition(int polanasId, int id){			//Sprawdza czy proces moze wejsc na dana polane
  unsigned int index = 0;
  int i = polany[polanasId][index]->type;
  while (i <= S) {
    if((i + polany[polanasId][index]->type) <= S) {
      if (id == polany[polanasId][index]->id) {
        return true;
      }
      i += polany[polanasId][index]->type;
    }
    index += 1;
  }
  return false;
}
Lamport::Lamport(int id, int polanyNumber) {
  this->id = id;
  this->clock = 0;
  for (int i = 0; i < polanyNumber; i++) {
    this->polany.push_back(vector<Demand *>(0));
  }
}
