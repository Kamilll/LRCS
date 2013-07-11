CPP_SRCS += \
${addprefix $(ROOT)/Wrappers/, \
BasicBlock.cpp \
BitBlock.cpp \
CodingException.cpp \
MultiBlock.cpp \
BitmapDataSource.cpp \
LZDataSource.cpp \
StringDataSource.cpp \
RLEBlock.cpp \
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


