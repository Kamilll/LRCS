CPP_SRCS += \
${addprefix $(ROOT)/UnitTests/, \
UnitTest.cpp \
Query1S.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
UnitTests/%.o: $(ROOT)/UnitTests/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '


