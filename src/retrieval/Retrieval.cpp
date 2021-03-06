//
// Created by slh on 17-9-6.
//

#include "Retrieval.h"
#include <fstream>
#include <index_io.h>
#include <AuxIndexStructures.h>
#include <fstream>

retrieval::FeatureIndex::FeatureIndex(int dimension, int nlist,
                                      int groups, int nbits):
    _dimension(dimension), _nlist(nlist), _groups(groups), _nbits(nbits)

{
    _quantizer = new faiss::IndexFlatL2(dimension);

    _index = new faiss::IndexIVFPQ(_quantizer, dimension, nlist, groups, nbits);
}

long retrieval::FeatureIndex::getTotalIndex() {
    if(_index != NULL) {
        return _index->ntotal;
    }else{
        return 0;
    }
}

void retrieval::FeatureIndex::WriteIndexToFile(const char* saveFileName){

    if(  _index == NULL || (!_index->is_trained) ){
        std::cout<<"Empty Index Without Training"<<std::endl;
        return ;
    }

    if( std::fstream(saveFileName, std::ios::in ) ){
        std::cout<<"File Already Exist"<<std::endl;
        return ;
    }

    faiss::write_index(this->_index, saveFileName);

}

void retrieval::FeatureIndex::ReadIndexFromFile(const char* fileName){

    if( !std::fstream(fileName, std::ios::in ) ){
        std::cout<<"File Not Exist"<<std::endl;
        return ;
    }

    _index = dynamic_cast<faiss::IndexIVFPQ*>(faiss::read_index(fileName, false));

    /// init variable
    _dimension = _index->d;
    _nprobe = _index->nprobe;
    _groups = _index->pq.M;
    _nbits = _index->pq.nbits;
    _nlist = _index->codes.size();
    _size = _index->ntotal;
}

void retrieval::FeatureIndex::AddItemToFeature(float* data){

    this->_index->add(1, data);

    _size = _index->ntotal;

}

void retrieval::FeatureIndex::DeleteItemFromFeature(int id) {

    // ID selector construct
    faiss::IDSelector* ids = new faiss::IDSelectorRange(id, id+1);

    this->_index->remove_ids(*ids);

    _size = _index->ntotal;

}

void retrieval::FeatureIndex::AddItemList(int numOfdata, float *data) {

    this->_index->add(numOfdata, data);

    _size = _index->ntotal;

}

void retrieval::FeatureIndex::DeleteItemList(int beginId, int numOfdata) {

    // ID selector construct
    faiss::IDSelector* ids = new faiss::IDSelectorRange(beginId, beginId + numOfdata);

    this->_index->remove_ids(*ids);

    _size = _index->ntotal;

}

void retrieval::FeatureIndex::RetievalIndex(int numOfquery, float* nquery,
                                            int Ktop, long* index, float* Distance) {

    _index->search(numOfquery, nquery, Ktop, Distance, index);

}

void retrieval::FeatureIndex::TrainIndex(int count, float *data) {

    _index->train(count, data);

}
