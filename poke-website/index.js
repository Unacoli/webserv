function p2_trigger()
{
    document.getElementById('p2').style.display = "flex";
    document.getElementById('home').style.display = "none";
    document.getElementById('p2').scrollIntoView();
}

function clmurphySpawn() {
    document.getElementById("clmurphypoke").style.visibility = "visible";
}

function clmurphyUnspawn() {
    document.getElementById("clmurphypoke").style.visibility = "hidden";
}

function barodrigSpawn() {
    document.getElementById("barodrigpoke").style.visibility = "visible";
}

function barodrigUnspawn() {
    document.getElementById("barodrigpoke").style.visibility = "hidden";
}


function vote()
{

    let name = prompt("Firstly, before voting, please enter your anme ");
    if (!name)
        name = prompt("I said enter your name !!");
    let age = prompt("We remind you that you must be over 18 to vote, what is your age ?");
    if (!age)
        age = prompt
    else if (age < 18 || age < 0)
    {
        alert(" Sorry too young to vote")
        return ;
    }
    let vote = prompt("Ok all good, so finally, who do you trust with the future of your nation ?");
    if (!vote)
        vote = prompt("It's the future of our nation here god dammit vote !!");

    if (name && age && vote)
    {
        fetch("/data/test.json", {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            }, 
            body: JSON.stringify({
                name: name,
                age: age,
                vote: vote
            })
        })
        .then(response => console.log("RESPONSE : ", response))
        .catch(error =>  console.log("ERRROR : ", error));

    }
}

function see_votes()
{
    console.log('hry');
}


window.onload=function(){

    const form= document.querySelector('form');
    
    form.addEventListener('submit', function(event) {
        event.preventDefault();
        const checkbox1 = document.querySelector('#checkbox1');
        const checkbox2 = document.querySelector('#checkbox2');
        const value = checkbox1.checked ? checkbox1.value : checkbox2.checked ?checkbox2.value : null;
        console.log(value);
        if (value)
        {
            console.log('here');
            const xhr = new XMLHttpRequest();
            xhr.open("POST", "/data/data", true);
            xhr.setRequestHeader("Content-Type", "text/plain");
            xhr.send(value);
        }
    })
  
}

function getResults()
{
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "data/data", true);
    xhr.onreadystatechange = function()
    {
        if(xhr.readyState === XMLHttpRequest.DONE && xhr.status === 200);
        console.log(xhr.responseText);
    };
    xhr.send();
}
