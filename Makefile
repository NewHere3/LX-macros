BUILD=builds
HEADER=headers
SOURCE=sources

all: LX-macros LX-user

LX-user: LX-user.o LX-messages.o
	g++ $(BUILD)/LX-user.o $(BUILD)/LX-messages.o -o LX-user

LX-user.o: LX-user.cpp
	g++ -c LX-user.cpp -o $(BUILD)/LX-user.o

LX-macros: LX-macros.o LX-input.o LX-listener.o LX-global_variables.o LX-overlay.o LX-active_window.o LX-run.o LX-messages.o LX-selections.o
	g++ $(BUILD)/LX-macros.o $(BUILD)/LX-input.o $(BUILD)/LX-listener.o $(BUILD)/LX-global_variables.o $(BUILD)/LX-overlay.o $(BUILD)/LX-active_window.o $(BUILD)/LX-selections.o $(BUILD)/LX-run.o $(BUILD)/LX-messages.o -o LX-macros -lX11

LX-macros.o: LX-macros.cpp
	g++ -c LX-macros.cpp -o $(BUILD)/LX-macros.o

%.o: $(SOURCE)/%.cpp  # $(HEADER)/%.hpp
	g++ -c $^ -o $(BUILD)/$@

clean:
	rm -f $(BUILD)/*.o LX-macros LX-user
