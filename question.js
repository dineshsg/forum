jQuery(document).ready(function($) {
    webroot = "http://"+location.hostname+"/iParent1/";
	
	$('.grade').change(function(){
	    grd = $('.grade').attr('value');
	    $.ajax({
	        type : 'POST',
	        url : webroot + "get_subject",
	        data : {
				'grade' : grd
			},
			success : function(data){
			    output = $.parseJSON(data);
			    var html_data = "<option val=''>-- Select --</option>";
			    if(output){
			        $.each($.parseJSON(data), function(idx, obj) {
			            html_data += "<option value='"+ obj.id +"'>" + obj.subject_name + "</option>";	
                    });
                    $('.subject').html(html_data);
			    }
			    else{
			        alert('Somthing wrong, Please refresh page');
			    }
			}
			
        });
        
    });
    
    $('.common').click(function() {
        var grd = $(this).attr('id');
		//var cnt_val=$('#hdn'+grd).val();
        $.ajax({
	        type : 'POST',
	        url : webroot + "get_subject",
	        data : {
				'grade' : grd
			},
			success : function(data1){
			    //alert(data);
			    output1 = $.parseJSON(data1);
			     var html_data = "",imgname='';
			    if(output1){
			        $.each($.parseJSON(data1), function(idx, obj) {
						if(obj.subject_name=='Drama')							
									imgname='Drama.png';
							if(obj.subject_name=='Study Skills')							
									imgname='StudySkills.png';							
							if(obj.subject_name=='Mathematics')							
									imgname='Mathematics.png';
							if(obj.subject_name=='Language Skills')							
									imgname='LanguageSkills.png';
							if(obj.subject_name=='Reading Development')							
									imgname='ReadingDevelopment.png';							
							if(obj.subject_name=='Writing Development')							
									imgname='WritingDevelopment.png';
							if(obj.subject_name=='Learning Styles')							
									imgname='LearningStyles.png';
							if(obj.subject_name=='Project preparation')							
									imgname='Projectpreparation.png';
							if(obj.subject_name=='Research Skills')							
									imgname='ResearchSkills.png';							
							if(obj.subject_name=='Sports & Recreation')							
									imgname='SportsRecreation.png';
							if(obj.subject_name=='Sciences')							
									imgname='Sciences.png';
							if(obj.subject_name=='Life Skills')							
									imgname='LifeSkills.png';
							if(obj.subject_name=='Music')							
									imgname='Music.png';
							if(obj.subject_name=='Visual Arts')							
									imgname='VisualArts.png';
								
			            /*html_data += "<span id='div"+obj.id+"' target='targetDiv' style='float:left;'><p id='"+ obj.id +"' style='border:1px solid #000;margin:25px;width:225px;background-color: #efefef;height:104px !important;'><a href='forum/questions/"+grd+"/"+obj.id+"/'>" + obj.subject_name + "</a></p></span>";
                    }); */
					html_data += "<span id='div"+obj.id+"' target='targetDiv' style='float:left;'><p id='"+ obj.id +"' style='border:1px solid #000;margin:25px;width:225px;height:110px !important;box-shadow: 10px 10px 5px #888888;background-image:url(./images/topics/"+imgname+") !important;background-repeat:no-repeat;background-position:center;text-align:center;' class='content-font'><a href='forum/questions/"+grd+"/"+obj.id+"/' class='font_black '>" + obj.subject_name + "</a></p></span>";	
					});//background-image:url(./images/grades/Drama.png);<img src='./images/topics/Drama.png' /> background-color: #efefef;
                    
                   $('.container_subjects').html(html_data);
				   $('#container_subjects').focus();					
                    window.scrollBy(0, 10000);
				 
			    }
			    else{
			        alert('Somthing wrong, Please refresh page');
			    }
			}
			
        });
    });
    
    $(".mobile").keypress(function (e) {
     //if the letter is not digit then display error and don't type anything
     if (e.which != 8 && e.which != 0 && (e.which < 48 || e.which > 57)) {
        //display error message
        $("#errmsg").html("<strong>Please enter proper mobile number.</strong>").show().fadeOut("slow");
               return false;
    }
   });
   $(".mobile").blur(function (e) {
        var mobile_len = $('.mobile').val();
        if (!($.trim(mobile_len).length == 10)) {
        //display error message
        $("#errmsg").html("<strong>Please enter 10 mobile number.</strong>").show().fadeOut("slow");
        $('.mobile').focus();
               return false;
    } 
   });
});
