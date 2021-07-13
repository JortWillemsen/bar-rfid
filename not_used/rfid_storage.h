#ifndef BAR_RFID_RFID_STORAGE_H
#define BAR_RFID_RFID_STORAGE_H

//RFID storage for person_data
class RFID_Storage{
private:
    person_data person_data_storage[255];
    int index = 0;

public:
    bool add(const person_data& id){
        if(index <= 255) {
            person_data_storage[index] = id;
            index++;
            return true;
        }
        return false;
    }

    signed int locate_id(const uint64_t & id){
        for(int i = 0; i < index; i++){
            if(person_data_storage[i].userData.id == id){
                return i;
            }
        }
        return -1;
    }

    person_data get_from_id(const uint64_t& id){
        signed int location = locate_id(id);
        if(location >= 0) {
            return person_data_storage[location];
        }return person_data();
    }
};

#endif //BAR_RFID_RFID_STORAGE_H
