function p2_trigger()
{
    document.getElementById('p2').style.display = "block";
    document.getElementById('p3').style.display = "none";
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

function p3_trigger()
{
    document.getElementById('p2').style.display = "none";
    document.getElementById('p3').style.display = "block";
    document.getElementById('p3').scrollIntoView();
}
