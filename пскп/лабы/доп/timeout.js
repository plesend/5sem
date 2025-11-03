async function sayHello() {
    console.log("hello");
    await new Promise(resolve => setTimeout(resolve, 1000)); 
    console.log("hhihihih");
}

console.log("Перед вызовом");
sayHello();
console.log("После вызова");
