var url;

// Execute a function when the user releases a key on the keyboard
document.addEventListener("keydown", function(event) {

  if (event.keyCode === 106) {
    url = "/api/set"
    event.preventDefault();
    document.getElementById("set").click();
  }
  if (event.keyCode === 111) {
    url = "/api/start"
    event.preventDefault();
    document.getElementById("start").click();
  }

}); 

form.addEventListener('submit', (event) => {
    // disable default action
    event.preventDefault();
    postdata();
    
});

function postdata(){
    // configure a request
    const xhr = new XMLHttpRequest();
    xhr.open('POST', url);
  
    var data = new FormData();
    data.append("clk" , document.getElementById("clocksel").value);
    data.append("h" , document.getElementById("hour").value);
    data.append("m" , document.getElementById("min").value);
    data.append("s" , document.getElementById("sec").value);
 
    console.log(data);
    xhr.send(data);
    // listen for `load` event
    xhr.onload = () => {
        document.getElementById("status").innerHTML=xhr.responseText;
        console.log(xhr.responseText);
    }
}


function isNumberKey1(evt){
    var charCode = (evt.which) ? evt.which : evt.keyCode
    if ((charCode > 95 && charCode < 106) || charCode == 8 )
        return true;
    if (charCode==107)
        document.getElementById("min").focus();
    if (charCode==109)
        document.getElementById("clocksel").focus();
    return false;
}
function isNumberKey2(evt){
    var charCode = (evt.which) ? evt.which : evt.keyCode
    if ((charCode > 95 && charCode < 106) || charCode == 8 )
        return true;
    if (charCode==107)
        document.getElementById("sec").focus();
    if (charCode==109)
        document.getElementById("hour").focus();
    return false;
}
function isNumberKey3(evt){
    var charCode = (evt.which) ? evt.which : evt.keyCode
    if ((charCode > 95 && charCode < 106) || charCode == 8 )
        return true;
    if (charCode==107)
        document.getElementById("set").focus();
    if (charCode==109)
        document.getElementById("min").focus();
    return false;
}
function tab1(evt){
    var charCode = (evt.which) ? evt.which : evt.keyCode
    if(charCode == 38 || charCode ==40 )
        return true;
    if (charCode==107)
        document.getElementById("hour").focus();
    if (charCode==109)
        document.getElementById("reset").focus();
    return false;
}

function tabsubmit1(evt){
    var charCode = (evt.which) ? evt.which : evt.keyCode
    if(charCode == 13){
        url = "/api/set"
        return true;
    }
    if (charCode > 95 && charCode < 106)
        return true;
    if (charCode==107)
        document.getElementById("start").focus();
    if (charCode==109)
        document.getElementById("sec").focus();
    return false;
}	

function tabsubmit2(evt){
    var charCode = (evt.which) ? evt.which : evt.keyCode
    if(charCode == 13){
        url = "/api/start"
        return true;
    }
    if (charCode > 95 && charCode < 106 )
        return true;
    if (charCode==107)
        document.getElementById("reset").focus();
    if (charCode==109)
        document.getElementById("set").focus();
    return false;
}

function tabsubmit3(evt){
    var charCode = (evt.which) ? evt.which : evt.keyCode
    if(charCode == 13){
        url = "/api/reset"
        return true;
    }
    if (charCode > 95 && charCode < 106 )
        return true;
    if (charCode==107)
        document.getElementById("clocksel").focus();
    if (charCode==109)
        document.getElementById("start").focus();
    return false;
}
