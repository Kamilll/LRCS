CPP_SRCS += \
${addprefix $(ROOT)/Wrappers/Decoder/, \
BitDecoder.cpp \
Decoder.cpp \
DeltaPosDecoder.cpp \
IntDecoder.cpp \
PosDecoder.cpp \
StringDecoder.cpp \
UncompressedDecoder.cpp \
RLEDecoder.cpp \
}

# Each subdirectory must supply rules for building sources it contributes
Wrappers/Decoder/%.o: $(ROOT)/Wrappers/Decoder/%.cpp
	@echo 'Building file: $<'
	@echo $(CC) $(CFLAGS) $(IFLAGS) -o $@ $< 
	@$(CC) $(CFLAGS) $(IFLAGS) -o $@ $< && \
	echo -n $(@:%.o=%.d) $(@D)/ > $(@:%.o=%.d) && \
	$(CC) $(RULEFLAGS) $(CFLAGS) $(IFLAGS) $< >> $(@:%.o=%.d)
	@echo 'Finished building: $<'
	@echo ' '
