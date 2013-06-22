CPP_SRCS += \
${addprefix $(ROOT)/Wrappers/, \
BasicBlock.cpp \
MultiBlock.cpp \
MultiSBlock.cpp \
PosMultiBlock.cpp \
BitBlock.cpp \
Type2Block.cpp \
PosType2Block.cpp \
CodingException.cpp \
DeltaPosBlock.cpp \
StringDataSource.cpp \
PosBasicBlock.cpp \
PosDeltaBlock.cpp \
PosRLEBlock.cpp \
RLEBlock.cpp \
PosArrayBlock.cpp\
SBasicBlock.cpp \
SMultiBlock.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Wrappers/%.o: $(ROOT)/Wrappers/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


