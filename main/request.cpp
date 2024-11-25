class Request(){
    public:
        Request(){
          this->;
        }

        void write(String data){
          const char *content = data.c_str();
          eepromClass.writeString(0, content);
          eepromClass.commit();
        };

        String get(char storedValue){
          return String(eepromClass.get(0,storedValue));
        };
    
    private:
        EEPROMClass eepromClass;
}