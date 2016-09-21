$marksDirectory = [System.IO.Path]::Combine([System.Environment]::CurrentDirectory, "marks")
if(![System.IO.Directory]::Exists($marksDirectory)){
    [System.IO.Directory]::CreateDirectory($marksDirectory)
}
[Reflection.Assembly]::LoadWithPartialName("System.Drawing")
$chars = '‚ ','‚¢','‚¤','‚¦','‚¨','‚©','‚«','‚­','‚¯','A','‚³','‚µ','‚·','‚¹','‚»','‚½','B','C','‚Ä','‚Æ','‚È','‚É','‚Ê','D','‚Ì','‚Í','‚Ð','‚Ó','E', 'G','‚Ü','‚Ý','‚Þ', 'H', '‚à',
    '‚â','I','‚ä','K','‚æ','‚ç','L','M','‚ê','‚ë','‚í','N','S','T','U'
$fnt = New-Object System.Drawing.Font("MS Gothic", 48)
$bck = New-Object System.Drawing.SolidBrush([System.Drawing.Color]::FromArgb(153, 98, 56))
foreach($c in $chars) {
    $bmp = New-Object System.Drawing.Bitmap(80, 80)
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.FillRectangle($bck,0, 0,80, 80) 
    $g.DrawString($c.ToString(), $fnt, [System.Drawing.Brushes]::Red, 0,0)
    $bmp.Save([System.IO.Path]::Combine($marksDirectory, $c.ToString() + ".png"), [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose()
    $bmp.Dispose()
}