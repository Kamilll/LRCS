CPP_SRCS += \
${addprefix $(ROOT)/Operators/, \
BlockPrinter.cpp \
ColumnExtracter.cpp \
SColumnExtracter.cpp \
PosOperator.cpp \
Operator.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Operators/%.o: $(ROOT)/Operators/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '

# DSM




