function servo1(val){
	var xhttp = new XMLHttpRequest();
	xhttp.open("GET", "states?servo1="+val, true);
	xhttp.send();
}
function servo2(val){
	var xhttp = new XMLHttpRequest();
	xhttp.open("GET", "states?servo2="+val, true);
	xhttp.send();
}
function servo3(val){
	var xhttp = new XMLHttpRequest();
	xhttp.open("GET", "states?servo3="+val, true);
	xhttp.send();
}
function start(){
	update("servo1");
	update("servo2");
	update("servo3");
}

function update(name){
	var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById(name).value = this.responseText;
    }
  };
  console.log("/"+name);
  xhttp.open("GET", "/"+name, true);
  xhttp.send();
}