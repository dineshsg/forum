<?php

function comment_waiting_for_approval()
    {
        global $user;
        $c_id = arg(3);
        $c_status = arg(4);
        if(($c_status==1)||($c_status==-1))
        {
            if($c_status==1)
            {
                $query_app = "UPDATE `forum_question_reply` SET `status`='".$c_status."', `updated` = CURRENT_TIMESTAMP WHERE id = '".$c_id."'";            
                if(db_query($query_app)){
                    drupal_set_message('New Comment has been posted on Forum');
                }else {
                    drupal_set_message('Error !!!!.Please try again.','error');
                }
                $question_detail = db_query("SELECT fq.grade_id, fq.subject_id, fq.question FROM forum_questions fq, forum_question_reply fqr WHERE fqr.id=".$c_id." AND fq.id = fqr.question_id")->fetchObject();
                $message = array('0'=>t('Dear User,<br><br>Your question has been replied on forum<br>
                                 Grade: '.grade($question_detail->grade_id).'<br>
                                 Topic: '.subject($question_detail->subject_id).'<br>
                                 Question: '.$question_detail->question.'<br>
                                 ')); 
                $params = array(
                    'body' => $message,
                    'subject' => 'Your Question Posted on Forum has been replied',
                );
                $question_id = db_query("SELECT question_id FROM forum_question_reply WHERE id = ".$c_id)->fetchObject();
                $stu_mail_id = student_mail($question_id->question_id);
                //$to = 'dineshsgaddi@gmail.com';
                $to = $stu_mail_id;
                $language = user_preferred_language($user);
                drupal_mail('forum', 'replytoquestion', $to, $language, $params, 'demo@demo.com', $send = TRUE);        
                drupal_goto('forum/comment/approval');
            }
            else
            {
                $query_rej = "UPDATE `forum_question_reply` SET `status`='".$c_status."', `updated` = CURRENT_TIMESTAMP WHERE id = '".$c_id."'";            
                if(db_query($query_rej)){
                    drupal_set_message('New Comment has been Rejected');
                }else {
                    drupal_set_message('Error !!!!.Please try again.');
                }
                $question_detail = db_query("SELECT fq.grade_id, fq.subject_id, fq.question, fq.user_id FROM forum_questions fq, forum_question_reply fqr WHERE fqr.id=".$c_id." AND fqr.question_id = fq.id")->fetchObject();
                $message = array('0'=>t('Dear Teacher,<br><br>Your question has been rejected by Moderator<br>
                                 Grade: '.grade($question_detail->grade_id).'<br>
                                 Topic: '.subject($question_detail->subject_id).'<br>
                                 Question: '.$question_detail->question.'<br>
                                 ')); 
                $params = array(
                    'body' => $message,
                    'subject' => 'Your Question Posted on Forum has been replied',
                );
                $question_id = db_query("SELECT user_id FROM forum_question_reply WHERE id = ".$c_id)->fetchObject();
                $teacher_mail_id = user_load($question_id->user_id);
                $to = $teacher_mail_id;
                $language = user_preferred_language($user);
                drupal_mail('forum', 'replytoquestion', $to, $language, $params, 'demo@demo.com', $send = TRUE);        
                drupal_goto('forum/comment/approval');
            }
            
        }
        else
        {
            if ((in_array('moderator', $user->roles)) || (in_array('admin moderator', $user->roles))||($user->uid == 1))
            {
                $output = '';
                $row = array();
                $header = array(
                            array('data' => 'Question', 'field' => 'question', 'sort' => 'asc'),
                            array('data' => 'Comment', 'field' => 'reply_message', 'sort' => 'asc'),
                            array('data' => 'Posted By', 'field' => 'user_id', 'sort' => 'asc'),
                            array('data' =>'Action', 'field' => '', 'sort' => 'asc')
                            );
                $result = db_select('forum_questions','fq');
                $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
                $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');
                $result->join('forum_question_reply','fqr', 'fq.id = fqr.question_id');
                $result->fields('fq', array('question','user_id','created'));
                $result->fields('fqr', array('reply_message','user_id','created','id','question_id'));
				$result->condition('fq.status',1,'=');
                /*$result = db_select('forum_questions','fq');
                $result->join('forum_question_reply','fqr', 'fqr.question_id = fq.id');
                $result->fields('fq', array('question'));
                $result->fields('fqr', array('reply_message','user_id','created','id','question_id'));
                $result->condition('fq.status',1,'=');*/
                if (in_array('moderator', $user->roles))
                {
                    $moderator_query = db_query('SELECT * FROM users_extend WHERE uid = '.$user->uid)->fetchObject();
                    $myArray = explode(',', $moderator_query->subject);
                    $result->condition('fq.subject_id', $myArray, 'IN');
                    $result->condition('fqr.status', 0)
                           ->execute()
                           ->fetchAll();
                }
                else if(in_array('admin moderator', $user->roles))
                {
                    $result->condition('fqr.status', 0)
                           ->execute()
                           ->fetchAll();
                }
                else if(in_array('administrator', $user->roles))
                {
                    $result->condition('fqr.status', 0)
                           ->execute()
                           ->fetchAll();
                }
                else
                {}
    		  if(isset($_REQUEST['sort']))
    		  {
    				$table_sort = $result->extend('TableSort')							
        						  		 ->orderByHeader($header);
                    $pager = $table_sort->extend('PagerDefault')  
                  	                	->limit(5);
		      }else{
                    $pager = $result->extend('PagerDefault')                  
          		                    ->orderBy('id', 'DESC')
                  	                ->limit(5);
		      }
		      $result = $pager->execute();
                if(!$result->rowCount())
                {
                    $output .= "No Comment for approval!!";
                }
                else
                {
                    while ($item = $result->fetch())
                    {
                        $user_post = user_load($item->user_id);
                        $username = $user_post->name;
                        $date_ago = ago($item->created);
                        $row[] = array('<a href="../comment-detail/'.$item->question_id.'/'.$item->id.'/">'.truncate_chars($item->question,25).'</a>',truncate_chars($item->reply_message,25),$username.'<div class="algn-right small-font">'.$date_ago.'</div>',l('Accept','forum/comment/approval/'.$item->id.'/1').' | '.l('Reject','forum/comment/approval/'.$item->id.'/-1'));
                    }
                    $output .= theme('table',array('header' => $header,'rows' => $row));
                    $output .= theme('pager');
                }
        	}
            else
            {
                drupal_set_message('Please ask your Moderator to provide access');
                $output = '';
            }
            return $output;
        }
    }
?>
