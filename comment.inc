<?php

function _comment()
    {
        global $user;
        global $base_url;
        $account = $user;
        $question_id = arg(2);
        $comment_id = arg(3);
        $output = '';
        if($question_id)
        {
            $question_detail = db_query('SELECT grds.grade_name, sub.subject_name, ques.question, ques.user_id, ques.created, ques.id FROM forum_grades as grds, forum_subjects as sub, forum_questions as ques WHERE  ques.id = '.$question_id.' AND ques.grade_id = grds.id and ques.subject_id = sub.id');		
		    $res_attachment = db_query('SELECT id,question_id,file_name from forum_question_attach where question_id='.$question_id.'');
            $question_reply = db_query('SELECT qr.reply_message, qr.id,qr.user_id,qr.created FROM forum_question_reply as qr WHERE qr.status = 0 AND qr.question_id = '.$question_id.'');
            $output .= "<div>";
  	        while($rows = $question_detail->fetchObject()){
			    $grade_name=$rows->grade_name;
			    $subject_name=$rows->subject_name;
		        $output .= "<div class='col-12'>
		                        <div class='img_bg border_blue'>
		                            <div class='space'>
  	                                    <div class='font-100 custom_font2 content_down '>
  	                                    Grade :<label class='font_black font-300'> ".$rows->grade_name." </label> &nbsp; &nbsp;              
  	                                    Topic :<label class='font_black font-300'> ".$rows->subject_name." </label>
  	                                </div>
  	                            </div>";
  	    
                $date_ago = ago($rows->created);
	            $temp_user = user_load($rows->user_id);
                $temp_username = $temp_user->name;
                $output .= "<div class='space content-font'>
                                <div>
                        	        <span>Posted By:</span><strong><span>".$temp_username."</span></strong>
                                </div>
                                <div>
                        	        <span>Date - </span><strong><span>".date('F jS Y', strtotime($rows->created))."</span></strong>
                                </div>";
		 			
                $output .= "<div class='content-font'>
                                <div>Question:</div>
                                <h5 class='font-300 font-black'><p style='text-indent: auto;'><strong>".nl2br($rows->question)."</strong></p></h5>";
				        		while($rows_attach = $res_attachment->fetchObject()){
		 		        			if($rows_attach->file_name)	
		 		        				$output .="<a href='../../".$rows_attach->file_name."' target='_blank' class='font_blue'>See file attached</a><br />";
				            		}
			    $output .="</div></div>";	
            }
        $question_detail1 = db_query('SELECT ques.grade_id, ques.subject_id, ques.user_id FROM forum_questions as ques WHERE ques.id = '.$question_id.'');
        $row = $question_detail1->fetchObject();
        $teacher_query = db_query("SELECT * FROM users_extend WHERE uid = '".$user->uid."' AND grade = '".$row->grade_id."' AND FIND_IN_SET('".$row->subject_id."', subject)");
        if($question_reply->rowCount() > 0)
	    {
            while($reply = $question_reply->fetchObject())
            {
				$reply_temp_user = user_load($reply->user_id);
				$reply_user = $reply_temp_user->name;
                $output .= "<div class='space content-font'>
                            <div>
                            <span>Replied By:</span><strong><span>".$reply_user."</span></strong>
                            </div>
                            <div>
                            <span>Date - </span><strong><span>".date('F jS Y', strtotime($reply->created))."</span></strong>
                            </div>";
                $output .= "<div id='form_{$reply->id}' class='form-wrapper'>";
                $output .= "<div>Answer:</div><h5 class='font-300 font-black'><strong>".nl2br($reply->reply_message)."</strong></h5>";
                $output .= "</div>";
                $output .= "</div><br>";
            }
			
        }
        if((in_array('admin moderator',$user->roles))||(in_array('moderator',$user->roles)))
        {
           $output.="<div class='col-12'><a href='".$base_url."/forum/comment/approval/".$comment_id."/1/' class='btn btn-default'>Accept</a> <a href='".$base_url."/forum/comment/approval/".$comment_id."/-1/' class='btn btn-default'>Reject</a></div>"; 
        }
        else
        {
                $output .= "<div class='space'>";
                $output .= drupal_render($form_question);
		        $prev_url=base_path()."forum/comment/".$row->grade_id."/".$row->subject_id;
		        $output .= "<a href='".$prev_url."'  class='btn btn-default content_down' > Go to ".$grade_name." &amp; ".$subject_name."</a><br><br>";
                $output .= "</div>";
        }
        $output .= "</div><br></div>";
        }
        else
        {
            $output .= 'Not A Valid Question';
        }
        return $output;
    }
?>
