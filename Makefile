.PHONY: all clean

OBJECTS = dw_get_accounting_data dw_stage_file dw_get_stripe_configuration

LDFLAGS += `pkg-config --cflags --libs cray-datawarp`

all:  $(OBJECTS)

clean:
	rm -fv $(OBJECTS)
