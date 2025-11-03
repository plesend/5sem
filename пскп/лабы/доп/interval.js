function sayHelloSync() {
    console.log("hello");

    setInterval(() => {
        console.log("hihihihihi");
    }, 1000);
}

console.log("Перед вызовом");
sayHelloSync();
console.log("После вызова");
