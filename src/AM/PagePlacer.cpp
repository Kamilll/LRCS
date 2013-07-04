#include "PagePlacer.h"

PagePlacer::PagePlacer(Encoder* src_, Decoder* decoder_, int numIndexes_, bool posPrimary_)
{
	setSrc(src_, decoder_, numIndexes_, posPrimary_);
	lastSecondaryValueSet=false;
	lastSecondaryValue=NULL; // whatever
	valueMap = new HashMap(2*TYPE2MAXVALUES);
	valueMap->setHashFunction(new IdentityHashFunction(2*TYPE2MAXVALUES));
	nameMap = new HashMap(2*TYPE2MAXVALUES);
	nameMap->setHashFunction(new IdentityHashFunction(2*TYPE2MAXVALUES));
}

PagePlacer::~PagePlacer()
{
	
}

void PagePlacer::setSrc(Encoder* src_, Decoder* decoder_, int numIndexes_, bool posPrimary_) {
	encoder=src_;
	decoder=decoder_;
	numIndexes=numIndexes_;

	//Temporary fix since we don't want to regenerate all code and pos should always be primary now:
	//posPrimary=posPrimary_;	
	posPrimary=true;

	if (numIndexes!=1 && numIndexes!=2) throw new AccessException("PagePlacer: page needs to have either 1 or 2 indices");
}

void PagePlacer::placeColumn(string name_, bool splitOnValue_, bool valSorted_) {
    valSorted = valSorted_;
	int page_size;
	if (!splitOnValue_) {
		Log::writeToLog("PagePlacer", 2, "NOT Spliting on Value");
		PageWriter* pageWriter=NULL;
		byte* page=encoder->getPage();
		while (page!=NULL) {
			page_size = encoder->getPageSize();
			decoder->setBuffer(page);
			if (!pageWriter) {
			  int primkeysize=sizeof(int);
			  int seckeysize=sizeof(int);
			  short primkeytype = ValPos::INTTYPE;
			  short seckeytype = ValPos::INTTYPE;
			  if (!posPrimary) {
			    primkeysize=decoder->getStartVal()->getSize();
			    primkeytype=decoder->getStartVal()->type;
			  }
			  if (posPrimary && numIndexes==2) {
			    seckeysize=decoder->getStartVal()->getSize();
			    seckeytype=decoder->getStartVal()->type;
			  }
			  pageWriter = new PageWriter(numIndexes, primkeysize, seckeysize, primkeytype, seckeytype);
			  pageWriter->initDB(name_, (valSorted?false:true));
			}	
			writePage(pageWriter, page, page_size, posPrimary, numIndexes);
			page=encoder->getPage();
		}
		pageWriter->closeDB();
		delete pageWriter;
	}
	else {
		Log::writeToLog("PagePlacer", 0, "Spliting on Value, with val page numIndexes=", valSorted_ ? 2 :1);
		PageWriter* valPageWriter=new PageWriter(valSorted_ ? 2 :1, sizeof(int), sizeof(int), ValPos::INTTYPE, ValPos::INTTYPE);
		valPageWriter->initDB(name_+"Vals", false);

		byte* page=encoder->getPage();
		if (page==NULL) Log::writeToLog("PagePlacer", 0, "Got NULL page at initialization");	
		int valsize = 0;
		short valtype = 0;
		ValPos* value=NULL;
		char* buffer;
		while (page!=NULL) {
			page_size = encoder->getPageSize();
			PageWriter* pageWriter=NULL;
			decoder->setBuffer(page);
			value=decoder->getStartVal();
			assert(valsize == 0 || value->getSize() == valsize);
			valsize = value->getSize();
			valtype = value->type;
			//Log::writeToLog("PagePlacer", 0, "Read a page, with value", value);			
			if ((pageWriter = (PageWriter*)valueMap->get(value)) == NULL) {
				pageWriter=new PageWriter(numIndexes, sizeof(int), sizeof(int), ValPos::INTTYPE, ValPos::INTTYPE);
				valueMap->put(value,pageWriter, valsize);
				ostringstream nameStream;
				nameStream << name_ << "ValsTemp";
				value->printVal(&nameStream);//<< value;
				Log::writeToLog("PagePlacer", 0, "Allocating new pageWriter, with name"+nameStream.str());
				pageWriter->initDB(nameStream.str(), false);
				buffer = new char[nameStream.str().size()+1];
				memcpy(buffer, nameStream.str().c_str(), nameStream.str().size());
				buffer[nameStream.str().size()] = '\0';
				nameMap->put(value,buffer,nameStream.str().size()+1);
			}
			/*else {
				Log::writeToLog("PagePlacer", 0, "Writing to pageWriter, with value", value);
				pageWriter=valueMap[value];	
				}*/
			
			// write the page with corresponding indexes
			writePage(pageWriter, page,page_size, true, 1);
	  		
	  		// get the next page
			page=encoder->getPage();
		}
		Log::writeToLog("PagePlacer", 0, "Pages of Values dry, (NULL returned by encoder), now writing value page");
		UncompressedEncoder* valEncoder=new UncompressedEncoder(NULL, 0, valsize, 8*PAGE_SIZE);
		UncompressedDecoder* valDecoder=new UncompressedDecoder(valSorted_,value->type);
		int currPos=1;
		
		ValPos* val = NULL;
		PageWriter* pw;
		ValPos** vparray = encoder->getValueIndex();
		int currindex = 0;
		bool doWhile = false;
		if (value->type == ValPos::INTTYPE) { // special case: vparray is empty so fill it
		  if (!valueMap->isEmpty()) {
		    val = valueMap->getKey()->clone();
		    doWhile = true;
		  }
		}
		else {
		  if (val = vparray[currindex++]) {
		    doWhile = true;
		  }
		}
		//while (!valueMap->isEmpty()) {
		while (doWhile) {
		  //delete val;
		  //val = valueMap->getKey()->clone();
		  pw = (PageWriter*) valueMap->remove(val);
		  //for (map<int, PageWriter*>::iterator pos=valueMap.begin(); pos!=valueMap.end(); ++pos) {
		  //int value=pos->first;
		  //(pos->second)->closeDB();
		  pw->closeDB();
		  delete pw;
		  //delete pos->second;
		  //Log::writeToLog("PagePlacer", 0, "Closed DB and delete PageWriter for value=", value);
		  assert(val->getSize()==valsize);
			if (valEncoder->writeVal((char*)val->value,currPos)) {
				currPos++;	
			}
			else {
				byte* page=valEncoder->getPage();
				valDecoder->setBuffer(page);
				//Log::writeToLog("PagePlacer", 2, "Writing Val page, w/ start value", valDecoder->getStartInt());
				//Log::writeToLog("PagePlacer", 2, "Writing Val page, w/ numValues", valDecoder->getSize());
				if (valSorted_) {
				  assert(false); //deal with this case later
				  int temppos = valDecoder->getEndPos();
				  valPageWriter->placePage((char*) page, (char*)valDecoder->getEndVal()->value,(char*)&temppos);
				}
				else { 
				  int temppos = valDecoder->getEndPos();
				  valPageWriter->placePage((char*) page, (char*)&temppos);				
				}
			}
			doWhile = false;
			if (value->type == ValPos::INTTYPE) { // special case: vparray is empty so fill it
			  if (!valueMap->isEmpty()) {
			    val = valueMap->getKey()->clone();
			    doWhile = true;
			  }
			}
			else {
			  if (val = vparray[currindex++]) {
			    doWhile = true;
			  }
			}
		}
		assert(valueMap->isEmpty());
		// write the remaining page
		page=valEncoder->getPage();
		valDecoder->setBuffer(page);
		//Log::writeToLog("PagePlacer", 2, "Writing Val page, w/ start value", valDecoder->getStartInt());
		//Log::writeToLog("PagePlacer", 2, "Writing Val page, w/ numValues", valDecoder->getSize());
		if (valSorted_) {
		  assert(false); //deal with this case later
		  int temppos = valDecoder->getEndPos();
		  valPageWriter->placePage((char*) page, (char*)valDecoder->getEndVal()->value,(char*)&temppos);
		}
		else {
		  int temppos = valDecoder->getEndPos();
		  valPageWriter->placePage((char*) page, (char*)&temppos);				
		}
		
		valPageWriter->closeDB();
		delete valPageWriter;
		Log::writeToLog("PagePlacer", 0, "Closed DB for Value page");
		ostringstream nameStream2;
		nameStream2 << "mv " << name_ << "* " RUNTIME_DATA;
		system(nameStream2.str().c_str());
		PageWriter* newWriter=new PageWriter(2, valsize, sizeof(int), valtype, ValPos::INTTYPE);
		ostringstream nameStream;
		nameStream << name_ << "ValsAll";
		newWriter->initDB(nameStream.str(), true);
		char* nm;
		//delete val;
		val = NULL;
		while (!nameMap->isEmpty()) {
		  delete val;
		  val = nameMap->getKey()->clone();
		  nm = (char*) nameMap->remove(val);
		  //for (map<int, string>::iterator pos=nameMap.begin();
		  //   pos!=nameMap.end();
		  //   ++pos) {
		  ROSAM* am = new ROSAM(nm, 1, sizeof(int), sizeof(int), ValPos::INTTYPE, ValPos::INTTYPE);
		  while ((page = (byte*)am->getNextPagePrimary()))
		    writePage(newWriter, page,page_size, false, 2);
		  delete am;
		  delete[] nm;
		}
		delete val;
		val = NULL;
		newWriter->closeDB();
		system(nameStream2.str().c_str());
		ostringstream nameStream3;
		nameStream3 << "rm " RUNTIME_DATA << name_ << "ValsTemp*";
		system(nameStream3.str().c_str());
	}
}

void PagePlacer::writePage(PageWriter* writer_, byte* page_, int page_size, bool posPrimary_, int numIndexes_) {
	bool useSecondary;
	decoder->setBuffer(page_);

	int temppos;

	//cout<<(char*)decoder->getEndVal()->value<<endl;
	if (numIndexes_==1) {
		if (posPrimary_) {
			temppos = decoder->getEndPos();
			writer_->placePage((char*) page_, (char*)&temppos,page_size);
		}
		else {	
			assert(valSorted);
			writer_->placePage((char*) page_, (char*)decoder->getEndVal()->value,page_size);
		}

	}	
	else if (numIndexes_==2) {
		if (valSorted){
			if (posPrimary_) {
				if (!lastSecondaryValueSet) {
					lastSecondaryValueSet = true;
					lastSecondaryValue = decoder->getEndVal()->clone();
					useSecondary=true;
				}else {
					ValPos* newSecVal = decoder->getEndVal();
					if (*newSecVal != lastSecondaryValue) {
						delete lastSecondaryValue;
						lastSecondaryValue = newSecVal->clone();
						useSecondary=true;
					}else
						useSecondary=false;
				}

				
				if (useSecondary) {
					temppos = decoder->getEndPos();
					writer_->placePage((char*) page_, (char*)&(temppos), (char*)lastSecondaryValue->value,page_size);
				}else {
					temppos = decoder->getEndPos();
					writer_->placePage((char*) page_, (char*)&(temppos),page_size);
				}
				
			}else {
				temppos = decoder->getEndPos();
				writer_->placePage((char*) page_, (char*)decoder->getEndVal()->value,(char*)&(temppos), page_size);
			}
		}else{//Value is unsorted:Create Position Primary and Value secondary
			//Postion priamry Index
			temppos = decoder->getEndPos();
			writer_->placePage((char*) page_, (char*)&temppos, page_size);
			//Value secondary Index
			while(decoder->hasNextBlock()){
				Block* outBlock = decoder->getNextBlock();
				while(outBlock->hasNext()){
					ValPos* vp = outBlock->getNext();
					temppos = vp->position;
					writer_->placeUnsortedRecord((char*)vp->value,(char*) &temppos);
				}
			}
		}
	}
}
