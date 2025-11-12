const { send } = require('m0603');

send("Это сообщение из установленного нпм модуля!")
    .then(() => console.log("Отправлено!"))
    .catch(err => console.error(err));