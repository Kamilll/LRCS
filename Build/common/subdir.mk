CPP_SRCS += \
${addprefix $(ROOT)/common/, \
Block.cpp \
DataSource.cpp \
Exception.cpp \
PosBlock.cpp \
PosFilterBlock.cpp \
MultiPosFilterBlock.cpp \
Predicate.cpp \
UnexpectedException.cpp \
UnimplementedException.cpp \
Writer.cpp \
ValPos.cpp \
IntValPos.cpp \
NullValPos.cpp \
LongValPos.cpp \
FloatValPos.cpp \
DoubleValPos.cpp \
StringValPos.cpp \
}
#Pair.cpp \


# Each subdirectory must supply rules for building sources it contributes
common/%.o: $(ROOT)/common/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


# DSM 
