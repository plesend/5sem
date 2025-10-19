const {EventEmitter} = require('events');

class DB extends EventEmitter {
    constructor() {
        super();
        this.table = [
            { id: 1, name: "name1", bday: "12.10.2025" },
            { id: 2, name: "name2", bday: "12.10.2025" },
            { id: 3, name: "name3", bday: "12.10.2025" },

        ];
        this.newId = 4;
        this.RegisterEventHandlers();
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

    RegisterEventHandlers = () => {
        this.on('GET', async(req,res) => {
            res.end(JSON.stringify(await this.select()));
        });

        this.on('POST', async(req, res, body) => {
            try {
                let newItem = JSON.parse(body);
                console.log("Post body: ", newItem)
                const inserted = await this.insert(newItem);
                res.end(JSON.stringify(inserted));
            }
            catch (err) {
                res.statusCode = 500;
                res.end(JSON.stringify({error: err.message}));
            }
        });

        this.on('PUT', async(req,res,body) => {
            try {
                let newItem = JSON.parse(body);
                let updItem = await this.update(newItem);
                res.end(JSON.stringify(updItem));
            }
            catch (err) {
                res.statusCode = 400;
                res.end(JSON.stringify({error:err.message}));
            }
        });

        this.on('DELETE', async(req, res, id) => {
            try {
                if (id === null || id === undefined) {
                    res.statusCode = 400;
                    res.setHeader('Content-Type', 'text/html')
                    res.end("<h1> DELETE: id is undefined");
                    return;
                }
                let delItem = await this.delete(id);
                res.end(JSON.stringify(delItem));
            }
            catch (err) {
                res.statusCode = 400;
                res.end(JSON.stringify({error:err.message}));
            }
        });
    }
}

module.exports = DB;
