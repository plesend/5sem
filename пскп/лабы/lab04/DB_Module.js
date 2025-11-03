const {EventEmitter} = require('events');

class DB extends EventEmitter {
    constructor() {
        super();
        this.table = [
            { id: 1, name: "name1", bday: "10.05.2006" },
            { id: 2, name: "name2", bday: "10.05.2006" },
            { id: 3, name: "name3", bday: "10.05.2006" },
        ];
    }

    async select() {
        return [...this.table];
    }

    async insert(item) {
        if (!item.id || !item.bday || !item.name) {
            throw new Error("1 property is missing");
        }
        if (this.table.some(row => row.id === item.id)) {
            throw new Error(`Insert: item with id ${item.id} already exists`);
        }

        let newItem ={ id: item.id, name: item.name, bday: item.bday };
        this.table.push(newItem);
        return newItem;
    }

    async update(updatedItem) {
        let toUpd = this.table.findIndex(item => item.id === updatedItem.id);
        if (toUpd === -1) throw new Error('Update: item not found');

        let current = this.table[toUpd];

        for (const key of Object.keys(updatedItem)) {
            if (updatedItem[key] !== undefined && updatedItem[key] !== null) {
                current[key] = updatedItem[key];
            }
        }

        return current;
    }

    async delete(id) {
        let toDel = this.table.findIndex(item => item.id === id);
        if (toDel === -1) throw new Error('Delete: item not found');

        let deleted = this.table.splice(toDel, 1)[0];

        return deleted;
    }

    async commit() {
        let rows = (await this.select()).length;
        return `${rows} have been successfully commited!`; 
    }
}

module.exports = DB;
