'use strict'

const { Database } = require('bindings')('sharedhashfile')

module.exports = (opts = {}) => {
    class SharedHashFile {
        
        constructor(opts = {}) {
    	  this._name = opts.name
    	  this._db = new Database()
        }

        get name() {
            return this._name
        }

        open() {
            return this._db.open(this._name)
        }

        close() {
            this._db.close()
        }

        size() {
            return this._db.size()
        }

        keys() {
            return this._db.keys()

        }

        get(key) {
            return this._db.get(key)

        }

        del(key) {
            return this._db.del(key)
            
        }

        put(key, value) {
            return this._db.put(key, value)
        }
        
    }
    return new SharedHashFile(opts);
}
