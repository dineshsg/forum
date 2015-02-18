<?php

    function forum()
    {
        $output ="<div class='col-12'><a href='ask-a-question' class='ctools-use-modal ctools-modal-forum-style'>Ask a question</a></div>";
        $search = drupal_get_form("forum_search");
        $output .= "<span>".drupal_render($search)."</span>";
        $query = 'SELECT DISTINCT fg.grade_name,fg.id FROM forum_grades fg, forum_subjects fs WHERE fg.id = fs.grade_id';
        $result = db_query($query);
        
        $count = 0;
        /*foreach ($result as $row)
        {
            //var_dump($row->grade_name);
            if($count)
            {
            $output .= "<div class='grade common' id='".$row->id."' target='".$row->id."'>
            <div class='thumb'>
        	<img data-src='holder.js/225x100/random/font:Droid Sans/text:".$row->grade_name."'>
        	<div class='caption'>
        		Placeholder with random theme
        	</div>
            </div>
            </div>";
            }
            else
            {
            $output .= "<div class='grade1 common' id= '".$row->id."' target='".$row->id."'>
            <div class='thumb'>
        	<img data-src='holder.js/225x100/random/font:Droid Sans/text:".$row->grade_name."'>
        	<div class='caption'>
        		Placeholder with random theme
        	</div>
            </div>
            </div>";
            }
            //$output .=".$row->subject_name.";
            $count++;
        }
        $output .= "<div class='container_subjects' style='clear:both;background:#efefef;! important;height:191px;width:96%;' ></div>";*/
		foreach ($result as $row)
        {
            //var_dump($row->grade_name);
			$img=$row->grade_name."png";
			switch($row->grade_name)
				{
					case "Pre-primary": 
									$img="primary.png";
									break;
					case "Junior Primary" : 
									$img="JuniorPrimary.png";
									break;
					case "Senior Primary" : 
									$img="SeniorPrimary.png";
									break;	
					case "High School" : 
									$img="HighSchool.png";
									break;	
					default : $img=$row->grade_name."png";
												
				}
            if($count)
            {
            /*$output .= "<div class='grade common' id='".$row->id."' target='".$row->id."'>
            <div class='thumb'>  	<img data-src='holder.js/225x100/random/font:Droid Sans/text:".$row->grade_name."'>
        	<div class='caption'>
        		Placeholder with random theme
        	</div>
            </div>
            </div>";			*/
			$output .= "<div class='grade1 common' id= '".$row->id."' target='".$row->id."'>
            <div class='thumb' style='background-image:url(./images/grades/".$img.");'>
        	&nbsp; 
            </div><div class='caption content-font'>
        		".$row->grade_name."
        	</div></div>";
            }
            else
            {
           /* $output .= "<div class='grade1 common' id= '".$row->id."' target='".$row->id."'>
            <div class='thumb'>
        	<img data-src='holder.js/225x100/random/font:Droid Sans/text:".$row->grade_name."'>
        	<div class='caption'>
        		Placeholder with random theme
        	</div>
            </div>
            </div>";*/
		$output .= "<div class='grade1 common' id= '".$row->id."' target='".$row->id."'>
            <div class='thumb' style='background-image:url(./images/grades/".$img.");'> 
        	&nbsp; 
            </div><div class='caption content-font'>
        		".$row->grade_name."
        	</div></div>";
			//<img data-src='holder.js/225x100/random/font:Droid Sans/text:".$row->grade_name."'>
            }
            //$output .=".$row->subject_name.";
			//$output .="<input type='hidden' id='hdn".$row->id."' name='".$count."' value='".$count."' />";
            $count++;
			/*if($count%4==0)
			{
				$divid="div".$count;
				$output .= "<div class='container_subjects' style='clear:both;background:#efefef;! important;height:191px;width:96%;' id='".$count."' ></div>";
			}*/
        }
		//$divid="div".$count;
        $output .= "<div class='container_subjects' style='clear:both;background:#efefef;! important;width:96%;
overflow:hidden;' id='container_subjects'  ></div>";//height:191px;  id='".$divid."'
        return $output;
    }
    
    function forum_search($form, &$form_state)
    {
        $form = array();
        $form['search'] = array(
		    '#type' => 'textfield',
		    '#size' => 30,
		    '#required' => TRUE,
		);
		$form["submit"] = array(
            '#type' => 'submit',
            '#value' => "Search",
            ''
        );
        return $form;
    }
    function forum_search_submit($form, &$form_state)
    {
        $output = "##########################";
        drupal_set_message('slkdjflksjd');     
        return $output;   
    }
?>
