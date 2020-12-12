function servo(num, val){
	var xhttp = new XMLHttpRequest();
	xhttp.open("GET", "?servo"+num+"="+val, true);
	xhttp.send();
}

function start(){
	var servos = 6;
	for(i=0;i<servos; i++) update("servo"+i);
}

function update(name){
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
    	document.getElementById(name).value = this.responseText;
		}
	};
	xhttp.open("GET", "/"+name, true);
	xhttp.send();
}
