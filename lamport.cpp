#include "lamport.h"

void Lamport::addProcess(Demand t, int polanasId){
  for (int i = 0; i < polany[polanasId].size(); i++) {
    if(polany[polanasId][i].clock < t.clock) {
      polany[polanasId].insert(polany[polanasId].begin()+i, t);
    }
  }
}

void Lamport::removeProcess(int id, int polanasId) {
  for(int i=0;i<polany[polanasId].size();i++) {
    if(polany[polanasId][i].id == id) {
      polany[polanasId].erase(polany[polanasId].begin() + i);
      break;
    }
  }
}

bool Lamport::checkPosition(int polanasId) {
  if(polany[polanasId].size()>0) {
    return polany[polanasId][0].id == id ? true : false;
  }
}

Lamport::Lamport(int id, int polanyNumber) {
  this->id = id;
  this->clock = 0;
  for (int i = 0; i < polanyNumber; i++) {
    this->polany.push_back(vector<Demand>(0));
  }
}
