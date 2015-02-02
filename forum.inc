<?php

    function forum()
    {
        $output ='';
        $query = 'SELECT DISTINCT fg.grade_name,fg.id FROM forum_grades fg, forum_subjects fs WHERE fg.id = fs.grade_id';
        $result = db_query($query);
        
        $count = 0;
        foreach ($result as $row)
        {
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
            $count++;
        }
        $output .= "<div class='container_subjects' style='clear:both;background:#efefef;! important;height:191px;width:96%;' ></div>";
        return $output;
    }
?>
