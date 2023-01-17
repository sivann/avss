//ftiaxnei ena megalo form me ola ta songs
//alla o IE exei kai pali limit sto size!
function addall() {
  var bigurl='',url;
  var frag;
//  playlist();	
  for (i=0;i<100;i++) {
      idname="add" + i;
      k = document.getElementById(idname);
      if (!k) { continue; }
      url=rightString(k.getAttribute("href"),"path=");
      bigurl=bigurl+url+'@@';

	  inp=document.createElement('input');
	  inp.type='text';
	  inp.name='ppp[]';
	  inp.value=url;
	  document.addallform.appendChild(inp);
	  t = document.createTextNode("\n");
	  document.addallform.appendChild(t);\


  }
  document.addallform.submit();
}
