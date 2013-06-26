BUILD_DIR := ../Build

debug:  forceMake cleanForceMake
	cd $(BUILD_DIR); make -k debug; cp cstoreqp ../src/cstoreqptest

#stop if error	
debug2:  forceMake cleanForceMake
	cd $(BUILD_DIR); make debug; cp cstoreqp ../src/cstoreqptest

clean:  forceMake cleanForceMake
	rm cstoreqptest; cd $(BUILD_DIR); make -k clean 

forceMake:
	touch forceMake

cleanForceMake:
	rm forceMake
