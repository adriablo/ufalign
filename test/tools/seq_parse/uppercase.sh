awk 'BEGIN{FS=" "}{if(!/>/){print toupper($0)}else{print $1}}' $1
